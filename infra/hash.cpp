#include "hash.h"

namespace infra {

uint32_t hash_32(const unsigned char *key, int32_t length, uint32_t seed) {
    uint32_t h;
    MurmurHash3_x86_32(key, length, seed, &h);
    return h;
}

uint32_t hash_str_32(const std::string &key, uint32_t seed) {
    return hash_32(reinterpret_cast<const unsigned char *>(key.c_str()), key.length(), seed);
}

} // namespace infra