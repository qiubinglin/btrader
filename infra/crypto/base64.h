#ifndef _BTRADE_CRYPTO_BASE64_H_
#define _BTRADE_CRYPTO_BASE64_H_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int base64encode(const char* buffer, size_t length, char** b64text);
int base64decode(char* b64message, unsigned char** buffer, size_t* length);

#ifdef __cplusplus
}
#endif

#endif