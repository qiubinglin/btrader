#include "py_journal.h"

#include <functional>
#include <unordered_map>

namespace btra {

static void on_bar(const EventSPtr &event, pybind11::dict &output) {
    const auto &bar = event->data<Bar>();
    std::cout << bar.open << std::endl;
}

typedef void (*HandleFunc)(const EventSPtr &, pybind11::dict &);
static const std::unordered_map<int32_t, HandleFunc> s_frame_cbs = {{MsgTag::Bar, &on_bar}};

void PyJournalComm::init(const std::string &conf_file) {
    std::ifstream f(conf_file);
    auto j = Json::json::parse(f);
    comm_data_.init(j);
}

void PyJournalComm::start() {
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

pybind11::dict PyJournalComm::read() {
    pybind11::dict res;

    auto &ob_helper = comm_data_.observe_helper;
    auto &reader = comm_data_.reader;
#ifndef HP
    if (ob_helper.data_available()) {
#endif
        while (reader->data_available()) {
            auto event = reader->current_frame();
            if (s_frame_cbs.contains(event->msg_type())) {
                s_frame_cbs.at(event->msg_type())(event, res);
            }
            reader->next();
        }
#ifndef HP
    }
#endif

    /* frame to dict */
    return res;
}

void PyJournalComm::write(const pybind11::dict &data) {
    /* dict to frame */
    /* Write frame */
}

} // namespace btra