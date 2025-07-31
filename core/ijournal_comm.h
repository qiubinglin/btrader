#pragma once

#include <functional>
#include <unordered_map>

#include "core/journal_comm_data.h"
#include "infra/log.h"

namespace btra {

class IJournalComm {
public:
    IJournalComm() {}
    void init(const std::string &conf_file) {
        std::ifstream f(conf_file);
        auto j = Json::json::parse(f);
        comm_data_.init(j);
    }

    void start() {
        /* tmp code */
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

    /**
     * @brief This method is used in python and cpp data interactive.
     *
     * @tparam TYPEINFO
     * @param output
     */
    template <typename TYPEINFO>
    void read_by_typeinfo(typename TYPEINFO::DataType &output) {
        INFRA_LOG_DEBUG("IJournalComm::read_by_typeinfo");

        auto &ob_helper = comm_data_.observe_helper;
        auto &reader = comm_data_.reader;
#ifndef HP
        if (ob_helper.data_available()) {
            INFRA_LOG_DEBUG("ob_helper.data_available()");
#endif
            while (reader->data_available()) {
                auto event = reader->current_frame();
                if (TYPEINFO::s_frame2datatype_cbs.contains(event->msg_type())) {
                    TYPEINFO::s_frame2datatype_cbs.at(event->msg_type())(event, output);
                }
                reader->next();
            }
#ifndef HP
        }
#endif
        return output;
    }

    template <typename TYPEINFO>
    void write_by_typeinfo(typename TYPEINFO::DataType &input) {
        /* Hash to get id */
        /* Call callback to write */
    }

    template <typename T>
    void write(uint32_t id, const T &data) {
        if (comm_data_.writers.count(id) == 0) [[unlikely]] {
            return;
        }
        comm_data_.writers[id]->write(infra::time::now_time(), data);
    }

private:
    JourCommData comm_data_;
};

} // namespace btra