#pragma once

#include <map>
#include <queue>

#include "core/enums.h"

namespace btra {

class RequestBook {
public:
    RequestBook() = default;

    void add_request(uint64_t req_id, enums::BrokerReqType req_type);

    void remove_request(uint64_t req_id);

    bool has_request(uint64_t req_id) const;

    enums::BrokerReqType get_request(uint64_t req_id) const;

private:
    std::map<uint64_t, std::pair<enums::BrokerReqType, uint64_t>> req_id2type_; // request id to request type and time.
};

} // namespace btra