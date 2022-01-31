#include <stdlib.h>
#include <errno.h>
#include <netinet/tcp.h>
#include <fcntl.h>
#include <signal.h>
#include <strings.h>

#include <openssl/ssl.h>
#include <openssl/err.h>


#define HOST	   "localhost"
#define PORT	    4433

#define TRUE               1
#define CERTIFICATE "cacert.pem"

extern BIO *bio_err;
int berr_exit (char *string);
int err_exit(char *string);

// Function Prototypes
SSL_CTX *initialize_ctx(char *certificate);
void destroy_ctx(SSL_CTX *ctx);


