#include "signer.h"

#include <openssl/err.h>
#include <openssl/pem.h>

#include <stdexcept>

#include "base64.h"
#include "ed25519.h"

namespace infra::crypto {

EncrypKey::EncrypKey(const std::string &pem_file, bool is_private) {
    FILE *fp = fopen(pem_file.c_str(), "r");
    if (!fp) {
        perror("Open private key failed");
        throw std::runtime_error("Open private key failed");
    }
    if (is_private) {
        pkey = PEM_read_PrivateKey(fp, NULL, NULL, NULL);
    } else {
        pkey = PEM_read_PUBKEY(fp, NULL, NULL, NULL);
    }
    fclose(fp);
    if (!pkey) {
        ERR_print_errors_fp(stderr);
        throw std::runtime_error("Create key failed");
    }
}

EncrypKey::~EncrypKey() {
    if (pkey)
        EVP_PKEY_free((EVP_PKEY *)pkey);
}

EncrypKey::EncrypKey(EncrypKey &&that) {
    pkey = that.pkey;
    that.pkey = NULL;
}

EncrypKey &EncrypKey::operator=(EncrypKey &&that) {
    pkey = that.pkey;
    that.pkey = NULL;
    return *this;
}

Signer::Signer(void *libctx) : libctx_(libctx) { init_ctx(); }

Signer::~Signer() { OSSL_LIB_CTX_free((OSSL_LIB_CTX *)libctx_); }

std::string Signer::sign(EncrypKey &api_secret, const std::string &msg, EncrypType type) {
    std::string sig;
    char *sig_value = NULL;
    size_t sig_len;
    if (sign(api_secret, msg.data(), msg.size(), &sig_value, &sig_len, type) == 0) {
        sig.resize(sig_len);
        memcpy(sig.data(), sig_value, sig_len);
    }
    OPENSSL_free(sig_value);
    return sig;
}

int Signer::sign(EncrypKey &api_secret, const char *msg, size_t msg_len, char **sig_value, size_t *sig_len,
                 EncrypType type) {
    unsigned char *raw_sig_value = NULL;
    size_t raw_sig_len = 0;
    int retval = 0;
    switch (type) {
    case EncrypType::ED25519:
    default: {
        if (!ed25519_sign((EVP_PKEY *)api_secret.pkey, (const unsigned char *)msg, msg_len, (OSSL_LIB_CTX *)libctx_,
                          &raw_sig_value, &raw_sig_len)) {
            retval = -1;
        }
    } break;
    }

    if (retval == 0) {
        int err = base64encode((char *)raw_sig_value, raw_sig_len, sig_value);
        *sig_len = err == 0 ? strlen(*sig_value) : 0;
        if (err != 0) {
            throw std::runtime_error("base64encode error!");
        }
    }
    OPENSSL_free(raw_sig_value);
    return retval;
}

int Signer::verify(EncrypKey &pub_key, const char *msg, size_t msg_len, const char *sig_value, size_t sig_len,
                   EncrypType type) {
    /* The sig_value should not be base64encode. */
    switch (type) {
    case EncrypType::ED25519:
    default: {
        if (!ed25519_verify((EVP_PKEY *)pub_key.pkey, (const unsigned char *)msg, msg_len,
                            (const unsigned char *)sig_value, sig_len, (OSSL_LIB_CTX *)libctx_)) {
            return -1;
        }
    } break;
    }
    return 0;
}

void Signer::init_ctx() {
    if (not libctx_)
        libctx_ = OSSL_LIB_CTX_new();
}

} // namespace infra::crypto