#pragma once

#include <pybind11/embed.h>

#include "broker/data_service.h"

namespace btra::broker {

class __attribute__((visibility("default"))) PyDataService : public DataService {
public:
    void setup(const Json::json &cfg) override;
    void start() override;
    void stop() override;

    bool subscribe(const std::vector<InstrumentKey> &instrument_keys) override;
    bool unsubscribe(const std::vector<InstrumentKey> &instrument_keys) override;

private:
    // pybind11::object impl_; to do use smart ptr
};

} // namespace btra::broker