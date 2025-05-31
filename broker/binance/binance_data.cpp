#include "binance_data.h"

namespace btra::broker {

const std::string BinanceData::s_ws_stream_prefix = "wss://stream.binance.com:443";

BinanceData::~BinanceData() {
    if (client_.is_connected()) {
        client_.close();
    }
}

void BinanceData::setup(const Json::json &cfg) {
    // "/stream?streams=btcusdt@trade/btcusdt@depth20@100ms"
    uri_ = s_ws_stream_prefix + "/stream?streams=btcusdt@kline_1s";

    client_.set_msg_handler([this](const std::string &msg) { this->on_msg(msg); });
}

void BinanceData::start() {
    if (client_.open(uri_)) {
        std::cout << "Connect binance md server failed!" << std::endl;
    }
}

void BinanceData::serve() {}

bool BinanceData::subscribe(const std::vector<InstrumentKey> &instrument_keys) { return true; }
bool BinanceData::unsubscribe(const std::vector<InstrumentKey> &instrument_keys) { return true; }

void BinanceData::on_msg(const std::string &msg) { std::cout << msg << std::endl; }

} // namespace btra::broker