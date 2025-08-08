#include "corecomm.h"

#include <filesystem>

#include "infra/log.h"

namespace btra {

void CoreComm::init(const std::string &conf_file) {
    if (std::filesystem::exists(conf_file)) {
        std::ifstream f(conf_file);
        auto j = Json::json::parse(f);
        comm_data_.init(j);
        inited_ = true;
    } else {
        INFRA_LOG_ERROR("When initial CoreComm: {} is not existed!", conf_file);
    }
}

void CoreComm::start() {
    /* tmp code */
    if (inited_) {
        auto &writers = comm_data_.writers;

        auto req_md_dest = journal::JIDUtil::build(journal::JIDUtil::MD_REQ);
        auto now_time = infra::time::now_time();

        TradingStart trading_start;
        trading_start.sync_time = now_time;
        writers[req_md_dest]->write(now_time, trading_start);

        for (auto &[key, writer] : writers) {
            if (key == req_md_dest) {
                continue;
            }
            writer->write(now_time, trading_start);
        }
    }
}

void CoreComm::register_handler(MsgTag::Tag tag, std::function<void(const EventSPtr &)> handler) {
    handlers_[tag] = handler;
}

void CoreComm::listening() {
    if (not inited_) {
        return;
    }
    INFRA_LOG_INFO("CoreComm::listening");

    auto &ob_helper = comm_data_.observe_helper;
    auto &reader = comm_data_.reader;
#ifndef HP
    while (ob_helper.data_available()) {
#endif
        while (reader->data_available()) {
            if (status_ < 0) {
                break;
            }
            auto event = reader->current_frame();
            if (handlers_.contains(event->msg_type())) {
                handlers_.at(event->msg_type())(event);
            }
            reader->next();
        }
#ifndef HP
        if (status_ < 0) {
            break;
        }
    }
#endif
}

void CoreComm::terminate() {
    status_ = -1;
    comm_data_.interrupt_sender.post();
}

} // namespace btra