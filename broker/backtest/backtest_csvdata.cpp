#include "backtest_csvdata.h"

namespace btra::broker {

void BackTestCSVData::setup(const Json::json &cfg) {
    filename_ = cfg["account"].get<std::string>();
    if (not reader_.set_file(filename_)) {
        throw std::runtime_error("read csv file failed");
    }
}

void BackTestCSVData::start() {
    std::vector<std::string> datas;
    bool met_header = false;
    while (not reader_.is_eof()) {
        reader_.read_row(datas);
        if (datas.empty()) {
            continue;
        }
        if (not met_header) {
            met_header = true;
            continue;
        }
        Bar bar;
        bar.close = std::stod(datas[1]);
        bar.high = std::stod(datas[2]);
        bar.low = std::stod(datas[3]);
        bar.open = std::stod(datas[4]);
        bar.volume = std::stol(datas[5]);
        infra::time::strptimerange(datas[0].c_str(), HISTORY_DAY_FORMAT, bar.start_time, bar.end_time);
        // INFRA_LOG_INFO("Bar Data: {} {} {} {} {} {} {} {}", bar.start_time, bar.end_time, bar.trading_day, bar.open,
        //                bar.high, bar.low, bar.close, bar.volume);
        writer_->write(infra::time::now_time(), bar);
    }
    writer_->write(infra::time::now_time(), Termination());
}

void BackTestCSVData::stop() {}

bool BackTestCSVData::subscribe(const std::vector<InstrumentKey> &instrument_keys) { return true; }

bool BackTestCSVData::unsubscribe(const std::vector<InstrumentKey> &instrument_keys) { return true; }

} // namespace btra::broker