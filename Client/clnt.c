/*---------------------------------------------------------------------------------------
--	SOURCE FILE:		tcp_clnt.c - A simple TCP client program.
--
--	PROGRAM:		muxc
--
--	FUNCTIONS:		Berkeley Socket API
--
--	DATE:			January 2, 2021
--
--	REVISIONS:		(Date and Description)
--				
--	DESIGNERS:		Aman Abdulla
--
--	PROGRAMMERS:	Aman Abdulla
--
--	NOTES:
--	The program will establish a TCP connection to a user specifed server.
-- The server can be specified using a fully qualified domain name or an
--	IP address. After the connection has been established the user will be
-- prompted for data. The data string is then sent to the server and the
-- response (echo) back from the server is displayed.
--

-- Compile: gcc -Wall -ggdb -o muxc mux_clnt.c
---------------------------------------------------------------------------------------*/
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<errno.h>
#include <netdb.h>
#include "common.h"


#define SERVER_TCP_PORT		8005	// Default port
#define BUFLEN			  1024  	// Buffer length
#define STDIN               0   // Standard input file descriptor
void connect_init(int *conn_sock, struct sockaddr_in *server, int port, char *host);


int main (int argc, char **argv)
{
	int conn_sock, port, i, err;
    int maxfd;
	struct sockaddr_in server;
	char  *host, *bp;
    fd_set start, read;

    // OpenSSL specific variables
    SSL_CTX *ctx;
    SSL *ssl;
    BIO *sbio;

	switch (argc)
	{
		case 2:
			host =	argv[1];	// Host name
			port =	SERVER_TCP_PORT;
		break;
		case 3:
			host =	argv[1];
			port =	atoi(argv[2]);	// User specified port
		break;
		default:
			fprintf(stderr, "Usage: %s host [port]\n", argv[0]);
			exit(1);
	}

    ctx = initialize_ctx(CERTIFICATE);
    ssl = SSL_new (ctx);

    connect_init(&conn_sock,&server,port,host);
    sbio = BIO_new_socket (conn_sock, BIO_NOCLOSE);
    SSL_set_bio (ssl, sbio, sbio);
    int r;
    if ((r= SSL_connect (ssl)) < 0) {

        int errr =SSL_get_error(ssl, r);
        printf("%d\n", errr);
        berr_exit("SSL Connect Error!");
    }
    // Get the localhost CA
    if(SSL_get_peer_certificate(ssl) != NULL)
    {
        if(SSL_get_verify_result (ssl) != X509_V_OK)
        {
            berr_exit("Could not verify peer certificate\n");
            printf ("Note: Could not verify peer certificate\n");
        }
    } else
    {
        berr_exit("Could not get peer certificate\n");
    }
    FD_ZERO(&start);
    FD_ZERO(&read);
    FD_SET(0, &start);
    FD_SET(conn_sock, &start);
    maxfd = conn_sock;


    while(1)
    {

        read = start;
        select(maxfd +1, &read, NULL, NULL, NULL);
        for(i = 0; i <= maxfd; i++)
        {
            if(FD_ISSET(i,&read))
            {
                char s_buff[BUFLEN];
                char r_buff[BUFLEN];
                ssize_t bytes_rec;
                if(i == 0)
                {
                    fgets(s_buff,BUFLEN,stdin);
                    if(strcmp(s_buff,"exit\n")==0)
                    {
                        printf("Thank you for using Bryan's chat!\n");
                        close (conn_sock);
                        exit(0);
                    }
                    else
                    {

                        err = SSL_write (ssl, s_buff, BUFLEN);
                        //send(conn_sock,s_buff, strlen(s_buff),0);
                    }
                }
                else
                {
                    bp = r_buff;
                    SSL_read (ssl, bp, BUFLEN);
                    //bytes_rec =recv(conn_sock,r_buff,BUFLEN,0);
                    //r_buff[bytes_rec]='\0';
                    printf("%s\n",r_buff);
                    fflush(stdout);
                }
            }
        }
    }
    printf("Client Closed\n");
	return (0);
}





void connect_init(int *conn_sock, struct sockaddr_in *server, int port, char *host) {
    char str[16];
    bzero ((char *)server, sizeof (struct sockaddr_in));
    server->sin_family = AF_INET;
    server->sin_port = htons (port);
    server->sin_addr.s_addr = inet_addr(host);
    //memset(server->sin_zero, '\0', sizeof server->sin_zero);
    // Create the socket
    if ((*conn_sock = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Cannot create socket");
        exit(1);
    }
    //

    struct hostent *hp;
    if ((hp = gethostbyname (host)) == NULL)
    {
        fprintf(stderr, "Unknown server address\n");
        exit(1);
    }
    //bcopy (hp->h_addr, (char *)&server->sin_addr, hp->h_length);
    //memset(server->sin_zero, '\0', sizeof server->sin_zero);
    // Connecting to the server
    if (connect (*conn_sock, (struct sockaddr *)server, sizeof(*server)) == -1)
    {
        fprintf(stderr, "Can't connect to server\n");
        perror("connect");
        exit(1);
    }
    printf ("Connected:    Server Name: %s\n", hp->h_name);
    char **pptr = hp->h_addr_list;
    printf ("\t\tIP Address: %s\n", inet_ntop(hp->h_addrtype, *pptr, str, sizeof(str)));
    fflush(stdout);
}
