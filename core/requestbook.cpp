#include "core/requestbook.h"

#include "infra/time.h"

namespace btra {

void RequestBook::add_request(uint64_t req_id, enums::BrokerReqType req_type) {
    req_id2type_[req_id] = {req_type, infra::time::now_time()};
}

void RequestBook::remove_request(uint64_t req_id) { req_id2type_.erase(req_id); }

bool RequestBook::has_request(uint64_t req_id) const { return req_id2type_.find(req_id) != req_id2type_.end(); }

enums::BrokerReqType RequestBook::get_request(uint64_t req_id) const {
    auto it = req_id2type_.find(req_id);
    if (it != req_id2type_.end()) {
        return it->second.first;
    }
    return enums::BrokerReqType::Unknown;
}

} // namespace btra