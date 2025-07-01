#include "broker/pyservice/pydata_service.h"

namespace btra::broker {

void PyDataService::setup(const Json::json &cfg) {}

void PyDataService::start() {}

void PyDataService::stop() {}

bool PyDataService::subscribe(const std::vector<InstrumentKey> &instrument_keys) { return true; }

bool PyDataService::unsubscribe(const std::vector<InstrumentKey> &instrument_keys) { return true; }

} // namespace btra::broker