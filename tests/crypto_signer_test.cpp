#include "infra/crypto/signer.h"

#include "infra/crypto/ed25519.h"

#include <string.h>

class SignerTest {
public:
    void ed25519_demo() {
        /* A test message to be signed (TBS) */
        static const char hamlet[] = "To be, or not to be, that is the question,\n"
                                     "Whether tis nobler in the minde to suffer\n"
                                     "The slings and arrowes of outragious fortune,\n"
                                     "Or to take Armes again in a sea of troubles,\n";

        OSSL_LIB_CTX *libctx = NULL;
        size_t sig_len = 0;
        unsigned char *sig_value = NULL;
        char *sig_value_ptr = NULL;
        int ret = EXIT_FAILURE;
        EVP_PKEY *priv = NULL, *pub = NULL;

        infra::crypto::EncrypKey priv_obj, pub_obj;
        infra::crypto::Signer signer;
        std::string signature;
        std::string msg;

        int err;

        libctx = OSSL_LIB_CTX_new();
        if (libctx == NULL) {
            fprintf(stderr, "OSSL_LIB_CTX_new() returned NULL\n");
            goto cleanup;
        }
        if (!ed25519_create_key(libctx, &priv, &pub)) {
            fprintf(stderr, "Failed to create key.\n");
            goto cleanup;
        }

        {
            priv_obj.pkey = priv;
            priv = NULL;
            pub_obj.pkey = pub;
            pub = NULL;

            signer.set_ctx(libctx);
            msg.resize(sizeof(hamlet));
            memcpy(msg.data(), hamlet, msg.size());
            signature = signer.sign(priv_obj, msg);
            signer.set_ctx(NULL);
            if (signature.empty()) {
                fprintf(stderr, "demo_sign failed.\n");
                goto cleanup;
            }

            signer.set_ctx(libctx);
            err = signer.verify(pub_obj, msg.data(), msg.size(), signature.data(), signature.size());
            signer.set_ctx(NULL);
            if (err) {
                fprintf(stderr, "demo_verify failed.\n");
                goto cleanup;
            }
        }

        // {
        //     priv_obj.pkey = priv;
        //     priv = NULL;

        //     signer.set_ctx(libctx);
        //     sig_value_ptr = (char *)sig_value;
        //     msg.resize(sizeof(hamlet));
        //     memcpy(msg.data(), hamlet, msg.size());
        //     err = signer.sign(priv_obj, msg.data(), msg.size(), &sig_value_ptr, &sig_len);
        //     signer.set_ctx(NULL);
        //     if (err) {
        //         fprintf(stderr, "demo_sign failed.\n");
        //         goto cleanup;
        //     }
        //     sig_value = (unsigned char *)sig_value_ptr;
        //     signature.resize(sig_len);
        //     memcpy(signature.data(), sig_value, sig_len);

        //     if (!ed25519_verify(pub, (const unsigned char *)hamlet, sizeof(hamlet),
        //                         (const unsigned char *)signature.data(), signature.size(), libctx)) {
        //         fprintf(stderr, "demo_verify failed.\n");
        //         goto cleanup;
        //     }
        // }

        // {
        //     if (!ed25519_sign(priv, (const unsigned char *)hamlet, sizeof(hamlet), libctx, &sig_value, &sig_len)) {
        //         fprintf(stderr, "demo_sign failed.\n");
        //         goto cleanup;
        //     }
        //     if (!ed25519_verify(pub, (const unsigned char *)hamlet, sizeof(hamlet), sig_value, sig_len, libctx)) {
        //         fprintf(stderr, "demo_verify failed.\n");
        //         goto cleanup;
        //     }
        // }

        ret = EXIT_SUCCESS;

    cleanup:
        if (ret != EXIT_SUCCESS)
            ERR_print_errors_fp(stderr);
        EVP_PKEY_free(pub);
        EVP_PKEY_free(priv);
        OSSL_LIB_CTX_free(libctx);
        OPENSSL_free(sig_value);
    }

    void test_ed25519() {
        /* Read private key. */
        /* Read Message to sign. */
        /* Read signature. */

        /* Do sign with Signer. */
        /* Compare result. */
    }
};

int main() {
    SignerTest test;
    test.ed25519_demo();
    return 0;
}