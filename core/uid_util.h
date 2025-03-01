#pragma once

#include <cstdint>

namespace btra::uidutil {

inline uint32_t to_account_uid(uint64_t order_id, uint32_t location_uid) { return (order_id >> 32u) xor location_uid; }

}  // namespace btra