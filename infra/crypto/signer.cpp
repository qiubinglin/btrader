#include "signer.h"

#include <openssl/err.h>
#include <openssl/pem.h>

#include <stdexcept>

#include "ed25519.h"

namespace infra::crypto {

EncrypKey::EncrypKey(const std::string &pem_file) {
    FILE *fp = fopen(pem_file.c_str(), "r");
    if (!fp) {
        perror("Open private key failed");
        throw std::runtime_error("Open private key failed");
    }
    pkey = PEM_read_PrivateKey(fp, NULL, NULL, NULL);
    fclose(fp);
    if (!pkey) {
        ERR_print_errors_fp(stderr);
        throw std::runtime_error("Create private key failed");
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
    switch (type) {
    case EncrypType::ED25519:
    default: {
        if (!ed25519_sign((EVP_PKEY *)api_secret.pkey, (const unsigned char *)msg, msg_len, (OSSL_LIB_CTX *)libctx_,
                          (unsigned char **)sig_value, sig_len)) {
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