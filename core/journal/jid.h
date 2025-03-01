#pragma once

#include <cstdint>
#include <unordered_map>

#include "infra/infra.h"

using JID = uint32_t;

namespace btra::journal {

namespace JIDUtil {

enum Flag : int8_t {
  MD_REQ,
  TD_RESPONSE,
};
inline JID build(JIDUtil::Flag flag) { return infra::hash_32((const unsigned char *)&flag, 1); }
inline JID build(const std::string &institution, const std::string &account_id) {
  return infra::hash_str_32(institution) xor infra::hash_str_32(account_id);
}

}  // namespace JIDUtil

}  // namespace btra::journal