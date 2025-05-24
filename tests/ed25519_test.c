#include "infra/crypto/ed25519.h"

/* A test message to be signed (TBS) */
static const unsigned char hamlet[] = "To be, or not to be, that is the question,\n"
                                      "Whether tis nobler in the minde to suffer\n"
                                      "The slings and arrowes of outragious fortune,\n"
                                      "Or to take Armes again in a sea of troubles,\n";

int main(void) {
    OSSL_LIB_CTX *libctx = NULL;
    size_t sig_len = 0;
    unsigned char *sig_value = NULL;
    int ret = EXIT_FAILURE;
    EVP_PKEY *priv = NULL, *pub = NULL;

    libctx = OSSL_LIB_CTX_new();
    if (libctx == NULL) {
        fprintf(stderr, "OSSL_LIB_CTX_new() returned NULL\n");
        goto cleanup;
    }
    if (!ed25519_create_key(libctx, &priv, &pub)) {
        fprintf(stderr, "Failed to create key.\n");
        goto cleanup;
    }

    if (!ed25519_sign(priv, hamlet, sizeof(hamlet), libctx, &sig_value, &sig_len)) {
        fprintf(stderr, "demo_sign failed.\n");
        goto cleanup;
    }
    if (!ed25519_verify(pub, hamlet, sizeof(hamlet), sig_value, sig_len, libctx)) {
        fprintf(stderr, "demo_verify failed.\n");
        goto cleanup;
    }
    ret = EXIT_SUCCESS;

cleanup:
    if (ret != EXIT_SUCCESS)
        ERR_print_errors_fp(stderr);
    EVP_PKEY_free(pub);
    EVP_PKEY_free(priv);
    OSSL_LIB_CTX_free(libctx);
    OPENSSL_free(sig_value);
    return ret;
}