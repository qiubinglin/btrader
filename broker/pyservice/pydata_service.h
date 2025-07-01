#pragma once

#include <pybind11/embed.h>

#include "broker/data_service.h"

namespace btra::broker {

class PyDataService : public DataService {
public:
    void setup(const Json::json &cfg) override;
    void start() override;
    void stop() override;

    bool subscribe(const std::vector<InstrumentKey> &instrument_keys) override;
    bool unsubscribe(const std::vector<InstrumentKey> &instrument_keys) override;

private:
    pybind11::object impl_;
};

} // namespace btra::broker