#pragma once

#include "broker/data_service.h"
#include "infra/csv.h"

namespace btra::broker {

class BackTestCSVData : public DataService {
public:
    void setup(const Json::json &cfg) override;
    void start() override;
    void stop() override;

    bool subscribe(const std::vector<InstrumentKey> &instrument_keys) override;
    bool unsubscribe(const std::vector<InstrumentKey> &instrument_keys) override;

private:
    infra::CSVReader reader_;
    std::string filename_;
};

} // namespace btra::broker