#pragma once

#include "data_service.h"

namespace btra::broker {

class BackTestData : public DataService {
 public:
  void setup(const Json::json &cfg) override;
  void start() override;
  void serve() override;

  bool subscribe(const std::vector<InstrumentKey> &instrument_keys) override;
  bool unsubscribe(const std::vector<InstrumentKey> &instrument_keys) override;

 private:
  infra::CSV csv_;
  std::string filename_;
};

}  // namespace btra::broker