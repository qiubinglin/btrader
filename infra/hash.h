#pragma once

#include <cstdint>
#include <string>

#include "MurmurHash3/MurmurHash3.h"

#define HASH_SEED 42

namespace infra {

uint32_t hash_32(const unsigned char *key, int32_t length, uint32_t seed = HASH_SEED);

uint32_t hash_str_32(const std::string &key, uint32_t seed = HASH_SEED);

} // namespace infra