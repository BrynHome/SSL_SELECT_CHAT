
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
static int  s_server_session_id_context = 1;
#define STDIN               0   // Standard input file descriptor
void connect_init(int *conn_sock, struct sockaddr_in *server, int port, char *host);

void * doRecieving(void * sockID){

    SSL *ssl = (SSL *) sockID;
    char *bp;
    struct timeval timeout;
    int rec, count = 0;
    while(1) {

        char r_buff[BUFLEN];
        bp = r_buff;
        SSL_read(ssl, bp, BUFLEN);
        printf("%s\n", r_buff);
        fflush(stdout);
    }





}


int main (int argc, char **argv) {
    int conn_sock, port, i, err;
    int maxfd;
    struct sockaddr_in server;
    char *host;
    fd_set start, read;

    // OpenSSL specific variables
    SSL_CTX *ctx;
    SSL *ssl;
    BIO *sbio;
    char name[40];
    gethostname(name,40);

    strcat(name,": ");
    switch (argc) {
        case 2:
            host = argv[1];    // Host name
            port = SERVER_TCP_PORT;
            break;
        case 3:
            host = argv[1];
            port = atoi(argv[2]);    // User specified port
            break;
        default:
            fprintf(stderr, "Usage: %s host [port]\n", argv[0]);
            exit(1);
    }

    ctx = initialize_ctx(CERTIFICATE);
    SSL_CTX_set_session_id_context(ctx, (void*)&s_server_session_id_context, sizeof s_server_session_id_context);
    ssl = SSL_new(ctx);



    connect_init(&conn_sock, &server, port, host);
    maxfd = conn_sock;
    SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);
    sbio = BIO_new_socket(conn_sock, BIO_CLOSE);
    //BIO_set_nbio(sbio, 0);
    SSL_set_bio(ssl, sbio, sbio);
    int r;
    if ((r = SSL_connect(ssl)) < 0) {

        int errr = SSL_get_error(ssl, r);
        printf("%d\n", errr);
        berr_exit("SSL Connect Error!");
    }

    long res;
    if (SSL_get_peer_certificate(ssl) != NULL) {
        if ((res = SSL_get_verify_result(ssl)) != X509_V_OK) {
            //berr_exit("Could not verify peer certificate\n");
            printf("Note: Could not verify peer certificate: %ld \n", res);
        }
    } else {
        berr_exit("Could not get peer certificate\n");
    }
    pthread_t thread;
    pthread_create(&thread, NULL, doRecieving, ssl);
    while (1) {


            char s_buff[BUFLEN];
            //
            char s_send[BUFLEN];
            strcpy(s_send, name);

            fgets(s_buff, BUFLEN, stdin);
            if (strcmp(s_buff, "exit\n") == 0) {
                printf("Thank you for using Bryan's chat!\n");
                close(conn_sock);
                exit(0);
            }
            //strcat(s_send,s_buff);

            err = SSL_write(ssl, s_buff, BUFLEN);

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
