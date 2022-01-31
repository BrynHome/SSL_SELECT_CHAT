/*--------------------------------------------------------------------------------------------------------------------
--	SOURCE FILE:	common.c - Miscellaneous SSL functions to build the SSL context and load all the crypto material
--
--	PROGRAM:		sclnt.exe
--
--	FUNCTIONS:		Berkeley Socket API, OPENSSL
--
--	DATE:			January 16, 2020
--
--	REVISIONS:		(Date and Description)
--
--
--	DESIGNERS:		Aman Abdulla
--
--
--	PROGRAMMERS:	Aman Abdulla
--
--	NOTES:
--
----------------------------------------------------------------------------------------------------------------*/

#include "common.h"

BIO *bio_err = 0;

// Basic error and exit routine
int err_exit (char *string)
{
    fprintf (stderr,"%s\n",string);
    exit(0);
}

// Print SSL errors and exit
int berr_exit(char *string)
{
    BIO_printf (bio_err,"%s\n",string);
    ERR_print_errors( bio_err);
    exit (0);
}

SSL_CTX *initialize_ctx (char *keyfile, char *certificate)
{
    SSL_METHOD *meth;
    SSL_CTX *ctx;

    if (!bio_err)
    {
        // Global system initialization
        SSL_library_init();
        SSL_load_error_strings();

        /* An error write context */
        bio_err = BIO_new_fp (stderr, BIO_NOCLOSE);
    }

    // Create our context
    meth = TLS_method();
    ctx = SSL_CTX_new (meth);

    // Load keys and certificates
    if (!(SSL_CTX_use_certificate_file (ctx, certificate, SSL_FILETYPE_PEM)))
        berr_exit ("Could not read server certificate file");


    if (!(SSL_CTX_use_PrivateKey_file (ctx, keyfile, SSL_FILETYPE_PEM)))
        berr_exit("Could not read private key file");

    return ctx;
}

void destroy_ctx (SSL_CTX *ctx)
{
    SSL_CTX_free (ctx);
}
