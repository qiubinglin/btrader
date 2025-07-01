#include "binance_data.h"

namespace btra::broker {

const std::string BinanceData::s_ws_stream_prefix = "wss://stream.binance.com:443";

BinanceData::~BinanceData() {
    if (client_.is_connected()) {
        client_.close();
    }
}

void BinanceData::setup(const Json::json &cfg) {
    // trade and depth: "/stream?streams=btcusdt@trade/btcusdt@depth20@100ms"
    /* kline: "/stream?streams=btcusdt@kline_1s" */
    uri_ = s_ws_stream_prefix + "/stream?streams=btcusdt@kline_1s/btcusdt@depth20";

    client_.set_msg_handler([this](const std::string &msg) { this->on_msg(msg); });
}

void BinanceData::start() {
    if (client_.open(uri_)) {
        std::cout << "Connect binance md server failed!" << std::endl;
    }
    INFRA_LOG_CRITICAL("Binance data service start!");
}

void BinanceData::stop() {}

bool BinanceData::subscribe(const std::vector<InstrumentKey> &instrument_keys) { return true; }
bool BinanceData::unsubscribe(const std::vector<InstrumentKey> &instrument_keys) { return true; }

void BinanceData::on_msg(const std::string &msg) {
    // std::cout << msg << std::endl;
    Json::json json_data = Json::json::parse(msg);
    auto mdtype = get_mdtype(json_data);
    switch (mdtype) {
    case enums::MDType::Kline: {
        const Json::json &kline_json = json_data["data"]["k"];
        Bar bar;
        bar.instrument_id = json_data["data"]["s"].get<std::string>().c_str();
        bar.start_time = kline_json["t"];
        bar.end_time = kline_json["T"];
        bar.open = std::stod(kline_json["o"].get<std::string>());
        bar.close = std::stod(kline_json["c"].get<std::string>());
        bar.high = std::stod(kline_json["h"].get<std::string>());
        bar.low = std::stod(kline_json["l"].get<std::string>());
        bar.volume = std::stod(kline_json["v"].get<std::string>());
        auto now_time = infra::time::now_time();
        writer_->write(now_time, bar);
    } break;
    case enums::MDType::Depth: {
        // std::cout << "Depth data: " << msg << std::endl;
        std::string price;
        std::string quantity;
        Quote depth;
        size_t idx = 0;
        for (const auto &item : json_data["data"]["bids"]) {
            if (idx >= depth.bid_price.length) {
                break;
            }
            price = item[0];
            quantity = item[1];
            depth.bid_price[idx] = std::stod(price);
            depth.bid_volume[idx] = std::stod(quantity);
            idx++;
        }

        idx = 0;
        for (const auto &item : json_data["data"]["asks"]) {
            if (idx >= depth.ask_price.length) {
                break;
            }
            price = item[0];
            quantity = item[1];
            depth.ask_price[idx] = std::stod(price);
            depth.ask_volume[idx] = std::stod(quantity);
            idx++;
        }
        auto now_time = infra::time::now_time();
        writer_->write(now_time, depth);
    } break;
    default:
        fprintf(stderr, "Unknown msg: %s\n", msg.c_str());
        break;
    }
}

enums::MDType BinanceData::get_mdtype(const Json::json &data) const {
    std::string stream_name = data["stream"].get<std::string>();
    if (stream_name == "btcusdt@kline_1s") {
        return enums::MDType::Kline;
    } else if (stream_name == "btcusdt@depth20") {
        return enums::MDType::Depth;
    } else {
        return enums::MDType::Unknown;
    }
}

} // namespace btra::broker