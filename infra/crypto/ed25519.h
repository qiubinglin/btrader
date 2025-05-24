#ifndef _BTRADE_CRYPTO_ED25519_H_
#define _BTRADE_CRYPTO_ED25519_H_

#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/core_names.h>

#ifdef __cplusplus
extern "C" {
#endif

int ed25519_create_key(OSSL_LIB_CTX *libctx, EVP_PKEY **privout, EVP_PKEY **pubout);

int ed25519_sign(EVP_PKEY *priv, const unsigned char *tbs, size_t tbs_len, OSSL_LIB_CTX *libctx,
                 unsigned char **sig_out_value, size_t *sig_out_len);

int ed25519_verify(EVP_PKEY *pub, const unsigned char *tbs, size_t tbs_len, const unsigned char *sig_value,
                   size_t sig_len, OSSL_LIB_CTX *libctx);

#ifdef __cplusplus
}
#endif

#endif