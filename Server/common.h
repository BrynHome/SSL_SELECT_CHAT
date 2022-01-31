//
// Created by root on 2022-01-30.
//

#ifndef SERVER_COMMON_H
#define SERVER_COMMON_H
#include <stdlib.h>
#include <errno.h>
#include <netinet/tcp.h>
#include <fcntl.h>
#include <signal.h>
#include <strings.h>

#include <openssl/ssl.h>
#include <openssl/err.h>

#define PORT	    8005
#define BUFSIZZ     1024
#define TRUE        1

#define KEYFILE "server_priv.key"
#define CERTIFICATE "server.cert"

extern BIO *bio_err;
int berr_exit (char *string);
int err_exit(char *string);

// Function Prototypes
SSL_CTX *initialize_ctx(char *keyfile, char *certificate);
void destroy_ctx (SSL_CTX *ctx);
#endif //SERVER_COMMON_H
