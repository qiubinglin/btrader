#pragma once

#include <functional>
#include <vector>

#include "infra/common.h"

namespace btra {

enum class NotifyType {
    Change,
    Update,
};

class Resource {
public:
    virtual ~Resource() {}

    void add_notice(std::function<void(NotifyType)> cb) { notice_cbs_.push_back(cb); }

    void notify_all(NotifyType type) {
        for (auto &cb : notice_cbs_) {
            cb(type);
        }
    }

private:
    std::vector<std::function<void(NotifyType)>> notice_cbs_;
};
DECLARE_SPTR(Resource);

} // namespace btra