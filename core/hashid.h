#pragma once

#include "core/types.h"
#include "infra/hash.h"

namespace btra {

inline uint32_t hash_instrument(const infra::Array<char, EXCHANGE_ID_LEN> &exchange_id,
                                const infra::Array<char, INSTRUMENT_ID_LEN> &instrument_id) {
    return infra::hash_str_32(instrument_id) ^ infra::hash_str_32(exchange_id);
}

inline uint32_t hash_product(const infra::Array<char, EXCHANGE_ID_LEN> &exchange_id,
                             const infra::Array<char, PRODUCT_ID_LEN> &product_id) {
    return infra::hash_str_32(product_id) ^ infra::hash_str_32(exchange_id);
}

} // namespace btra