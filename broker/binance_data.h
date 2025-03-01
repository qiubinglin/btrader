#pragma once

#include "data_service.h"
#include "infra/websocket.h"

namespace btra::broker {

class BinanceData : public DataService {
 public:
  void setup(const Json::json &cfg) override;
  void start() override;
  void serve() override;

  bool subscribe(const std::vector<InstrumentKey> &instrument_keys) override;
  bool unsubscribe(const std::vector<InstrumentKey> &instrument_keys) override;

 private:
  void on_msg(const std::string &msg);

  infra::WebSocketClient client_;
  std::string uri_;
};

}  // namespace btra::broker