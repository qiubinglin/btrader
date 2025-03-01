#include "binance_data.h"

namespace btra::broker {

void BinanceData::setup(const Json::json &cfg) {
  uri_ = fmt::format("wss://ws-api.binance.com:9443/ws-api/v3");

  client_.set_msg_handler([this](const std::string &msg) { this->on_msg(msg); });
}

void BinanceData::start() {
  client_.connect(uri_);
  client_.run();
}

void BinanceData::serve() {}

bool BinanceData::subscribe(const std::vector<InstrumentKey> &instrument_keys) { return true; }
bool BinanceData::unsubscribe(const std::vector<InstrumentKey> &instrument_keys) { return true; }

void BinanceData::on_msg(const std::string &msg) {}

}  // namespace btra::broker