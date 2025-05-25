#pragma once

#include <string>

namespace infra::crypto {

struct EncrypKey {
    void *pkey{nullptr};

    EncrypKey() {}
    EncrypKey(void *pkey) : pkey(pkey) {}
    EncrypKey(const std::string &pem_file, bool is_private);
    EncrypKey(const EncrypKey &that) = delete;
    EncrypKey(EncrypKey &&that);
    EncrypKey &operator=(EncrypKey &&that);
    ~EncrypKey();
};

enum class EncrypType {
    HMAC,
    RSA256,
    ED25519,
};

class Signer {
public:
    Signer() {}
    Signer(void *libctx);
    ~Signer();
    std::string sign(EncrypKey &api_secret, const std::string &msg, EncrypType type = EncrypType::ED25519);
    int sign(EncrypKey &api_secret, const char *msg, size_t msg_len, char **sig_value, size_t *sig_len,
             EncrypType type = EncrypType::ED25519);

    int verify(EncrypKey &pub_key, const char *msg, size_t msg_len, const char *sig_value, size_t sig_len,
               EncrypType type = EncrypType::ED25519);

    void init_ctx();
    void set_ctx(void *libctx) { libctx_ = libctx; }

private:
    void *libctx_{nullptr};
};

} // namespace infra::crypto