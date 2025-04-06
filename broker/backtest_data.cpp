#include "backtest_data.h"

namespace btra::broker {

void BackTestData::setup(const Json::json &cfg) { filename_ = cfg["file"].get<std::string>(); }

void BackTestData::start() {
    if (not csv_.read(filename_)) {
        throw std::runtime_error("read csv file failed");
    }
}

void BackTestData::serve() {
    for (size_t i = 0; i < csv_.row_count(); ++i) {
        const auto &row_data = csv_.get_row(i);
        Bar bar;
        bar.start_time = std::stoull(row_data[0]);
        bar.end_time = std::stoull(row_data[1]);
        bar.open = std::stod(row_data[2]);
        bar.close = std::stod(row_data[3]);
        bar.high = std::stod(row_data[4]);
        bar.low = std::stod(row_data[5]);
        writer_->write(infra::time::now_time(), bar);
    }
}

bool BackTestData::subscribe(const std::vector<InstrumentKey> &instrument_keys) {
    serve();
    return true;
}
bool BackTestData::unsubscribe(const std::vector<InstrumentKey> &instrument_keys) { return true; }

} // namespace btra::broker