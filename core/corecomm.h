#pragma once

#include <cstdint>
#include <functional>
#include <unordered_map>

#include "core/journal_comm_data.h"
#include "core/types.h"
#include "event.h"
#include "infra/log.h"

namespace btra {

class CoreComm {
public:
    CoreComm() {}
    void init(const std::string &conf_file);

    void start();

    void register_handler(MsgTag::Tag tag, std::function<void(const EventSPtr &)> handler);

    void listening();

    template <typename T>
    void write(uint32_t id, const T &data) {
        if (comm_data_.writers.count(id) == 0) [[unlikely]] {
            return;
        }
        comm_data_.writers[id]->write(infra::time::now_time(), data);
    }

    template <typename MSGTARGET, typename T>
    void write(const MSGTARGET &target, const T &data) {
        write(target(), data);
    }

private:
    JourCommData comm_data_;
    std::unordered_map<int, std::function<void(const EventSPtr &)>> handlers_;
};

} // namespace btra