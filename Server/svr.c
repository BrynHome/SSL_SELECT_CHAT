/*---------------------------------------------------------------------------------------
--	SOURCE FILE:		mux_svr.c -   A simple multiplexed echo server using TCP
--
--	PROGRAM:		muxs
--
--	FUNCTIONS:		Berkeley Socket API
--
--	DATE:			January 25, 2021
--
--	REVISIONS:		(Date and Description)
--				
--
--
--	DESIGNERS:	Aman Abdulla
--
--				
--	PROGRAMMER:		Aman Abdulla
--
--	NOTES:
--	The program will accept TCP connections from multiple client machines.
-- 	The program will read data from each client socket and simply echo it back.

-- Compile: gcc -Wall -ggdb -o muxs mux_svr.c
---------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <strings.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include "common.h"

#define SERVER_TCP_PORT 8005	// Default port
#define BUFLEN	1024	        //Buffer length
#define LISTENQ	5
#define KEYFILE "server_priv.key"
#define CERTIFICATE "server.cert"

// Function Prototypes
static void SystemFatal(const char* );
void connection_init(int *sockfd, struct sockaddr_in *server, int port);
void new_connection(fd_set *start, struct sockaddr_in *client, int *maxfd, int sockfd);
void send_receive(fd_set *start, int sockfd, int maxfd, int i,struct sockaddr_in *client);

static int  s_server_session_id_context = 1;

int main (int argc, char **argv)
{
    fd_set start, read;
	int i, maxi, port, maxfd, client[FD_SETSIZE];
	int sockfd= 0;
    socklen_t client_len;
	struct sockaddr_in server, client_addr;
   	ssize_t n;

    // OpenSSL specific variables
    SSL_CTX *ctx;      // SSL context structure
    SSL *ssl;          // SSL structures
    BIO *sbio = NULL;  // Initialize BIO Object

    switch(argc)
    {
        case 1:
            port = SERVER_TCP_PORT;	// Use the default port
            break;
        case 2:
            port = atoi(argv[1]);	// Get user specified port
            break;
        default:
            fprintf(stderr, "Usage: %s [port]\n", argv[0]);
            exit(1);
    }
    //ctx = initialize_ctx(KEYFILE,CERTIFICATE);
    //SSL_CTX_set_session_id_context(ctx, (void*)&s_server_session_id_context, sizeof s_server_session_id_context);
    maxi	= -1;
    for (i = 0; i < FD_SETSIZE; i++)
        client[i] = -1;

    FD_ZERO(&start);
    FD_ZERO(&read);
    connection_init(&sockfd,&server,port);
    FD_SET(sockfd, &start);
    maxfd = sockfd;
    while (1)
    {
        read = start;
        select(maxfd+1,&read,NULL,NULL,NULL);
        for(i=0;i<=maxfd;i++)
        {
            if(FD_ISSET(i,&read))
            {
                if(i == sockfd)
                {
                    new_connection(&start,&client_addr,&maxfd,sockfd);
                }
                else
                {
                    send_receive(&start,sockfd,maxfd,i,&client_addr);
                }
            }
        }
    }
	return(0);
}

// Prints the error stored in errno and aborts the program.
static void SystemFatal (const char* message)
{
    perror (message);
    exit (EXIT_FAILURE);
}

void connection_init(int *sockfd, struct sockaddr_in *server, int port)
{
    if ((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        SystemFatal("Cannot Create Socket!");
    }
    int arg = 1;
    if (setsockopt (*sockfd, SOL_SOCKET, SO_REUSEADDR, &arg, sizeof(arg)) == -1)
        SystemFatal("setsockopt");
    bzero((char *)server, sizeof(struct sockaddr_in));
    server->sin_family = AF_INET;
    server->sin_port = htons(port);
    server->sin_addr.s_addr = htonl(INADDR_ANY);
    memset(server->sin_zero,'\0', sizeof(server->sin_zero));

    if (bind (*sockfd, (struct sockaddr *)server, sizeof(*server)) == -1)
        SystemFatal("bind error");
    if(listen(*sockfd, LISTENQ)==-1){
        SystemFatal("listen");
    }
    printf("Waiting on %ui port %i\n", server->sin_addr.s_addr, port);
    fflush(stdout);
}

void new_connection(fd_set *start, struct sockaddr_in *client, int *maxfd, int sockfd)
{
    socklen_t client_len = sizeof(*client);
    int new_sd;
    if ((new_sd = accept(sockfd, (struct sockaddr *)client, &client_len)) == -1)
    {
        SystemFatal("accept error");
    } else
    {
        FD_SET(new_sd, start);
        if (new_sd > *maxfd)
        {
            *maxfd = new_sd;	// for select
        }
    }
    printf(" Remote Address:  %s\n", inet_ntoa(client->sin_addr));
}

void send_receive(fd_set *start, int sockfd, int maxfd, int i, struct sockaddr_in *client)
{
    int j;
    ssize_t bytes;
    char rec_buff[BUFLEN];
    if((bytes=recv(i,rec_buff,BUFLEN,0)) <= 0)
    {
        if(bytes ==0)
        {
            printf(" Remote Address:  %s closed connection\n", inet_ntoa(client->sin_addr));

        } else
            SystemFatal("recv");
        close(i);
        FD_CLR(i, start);
    } else
    {
        for(j=0;j<=maxfd;j++)
        {
            if(FD_ISSET(j,start))
            {
                if(j != sockfd && j != i) {
                    if(send(j,rec_buff,bytes,0) == -1)
                    {
                        printf("perror");
                    }
                }
            }
        }
    }
}



