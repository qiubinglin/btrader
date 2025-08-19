#pragma once

#include <memory>

#include "broker/data_service.h"
#include "infra/csv.h"

namespace btra::broker {

class FileDataService : public DataService {
public:
    void setup(const Json::json &cfg) override;
    void start() override;
    void stop() override;

    bool subscribe(const std::vector<InstrumentKey> &instrument_keys) override;
    bool unsubscribe(const std::vector<InstrumentKey> &instrument_keys) override;

    bool handle_backtest_sync_signal(const BacktestSyncSignal &signal) override;

private:
    std::unique_ptr<infra::CSVReader> reader_;
    std::string filename_;
    std::vector<std::string> row_buffer_;
    size_t row_count_{0};
};

} // namespace btra::broker