#include "binance_data.h"

#include <chrono>
#include <thread>

#include "infra/log.h"

namespace btra::broker {

const std::string BinanceData::s_ws_stream_prefix = "wss://stream.binance.com:443";

BinanceData::BinanceData() {}

BinanceData::~BinanceData() {
    stop();
}

void BinanceData::setup(const Json::json& cfg) {
    try {
        // Load configuration
        if (cfg.contains("extra")) {
            const auto& extra = cfg["extra"];

            // Stream URI configuration
            if (extra.contains("stream_uri")) {
                uri_ = extra["stream_uri"].get<std::string>();
            } else {
                set_default_streams();
            }

            // Symbol configuration
            if (extra.contains("symbol")) {
                default_symbol_ = extra["symbol"].get<std::string>();
            }

            // Stream type configuration
            if (extra.contains("enable_kline")) {
                enable_kline_ = extra["enable_kline"].get<bool>();
            }
            if (extra.contains("enable_depth")) {
                enable_depth_ = extra["enable_depth"].get<bool>();
            }

            // K-line interval configuration
            if (extra.contains("kline_interval")) {
                kline_interval_ = extra["kline_interval"].get<std::string>();
            }

            // Depth levels configuration
            if (extra.contains("depth_levels")) {
                depth_levels_ = extra["depth_levels"].get<int>();
            }

            // Reconnection configuration
            if (extra.contains("reconnect_interval_ms")) {
                reconnect_interval_ms_ = extra["reconnect_interval_ms"].get<int>();
            }
            if (extra.contains("max_reconnect_attempts")) {
                max_reconnect_attempts_ = extra["max_reconnect_attempts"].get<int>();
            }
        } else {
            set_default_streams();
        }

        INFRA_LOG_INFO("BinanceData configured: symbol={}, kline={}, depth={}, uri={}", default_symbol_, enable_kline_,
                       enable_depth_, uri_);

        // Initialize WebSocket client with event handlers
        client_.set_msg_handler([this](const std::string& msg) { this->on_msg(msg); });
        client_.on_open([this]() { this->on_connect(); });
        client_.on_close([this]() { this->on_disconnect(); });

    } catch (const std::exception& e) {
        INFRA_LOG_ERROR("Failed to setup BinanceData: {}", e.what());
        set_default_streams();
    }
}

void BinanceData::start() {
    try {
        INFRA_LOG_INFO("Starting Binance data service...");

        if (client_.open(uri_)) {
            INFRA_LOG_ERROR("Failed to connect to Binance WebSocket server: {}", uri_);
            should_reconnect_ = true;
            return;
        }

        is_connected_ = true;
        should_reconnect_ = false;
        current_reconnect_attempts_ = 0;
        INFRA_LOG_INFO("Successfully connected to Binance WebSocket server");

    } catch (const std::exception& e) {
        INFRA_LOG_ERROR("Exception during Binance data service start: {}", e.what());
        should_reconnect_ = true;
    }
}

void BinanceData::stop() {
    INFRA_LOG_INFO("Stopping Binance data service...");
    should_reconnect_ = false;
    is_connected_ = false;

    if (client_.is_connected()) {
        client_.close();
    }

    INFRA_LOG_INFO("Binance data service stopped");
}

bool BinanceData::subscribe(const std::vector<InstrumentKey>& instrument_keys) {
    try {
        if (instrument_keys.empty()) {
            INFRA_LOG_WARN("No instruments provided for subscription");
            return false;
        }

        if (!is_connected_) {
            INFRA_LOG_ERROR("Cannot subscribe: WebSocket not connected");
            return false;
        }

        INFRA_LOG_INFO("Subscribing to {} instruments", instrument_keys.size());

        // Build subscription message for each instrument
        std::vector<std::string> streams;
        std::vector<std::string> symbols;

        for (const auto& key : instrument_keys) {
            std::string symbol = key.instrument_id.to_string();
            std::transform(symbol.begin(), symbol.end(), symbol.begin(), ::tolower);

            // Add to symbols list if not already present
            if (std::find(symbols.begin(), symbols.end(), symbol) == symbols.end()) {
                symbols.push_back(symbol);

                // Build streams based on configuration
                if (enable_kline_) {
                    streams.push_back(symbol + "@kline_" + kline_interval_);
                }

                if (enable_depth_) {
                    streams.push_back(symbol + "@depth" + std::to_string(depth_levels_));
                }
            }
        }

        if (streams.empty()) {
            INFRA_LOG_WARN("No streams configured for subscription");
            return false;
        }

        // Build subscription message
        Json::json subscribe_msg;
        subscribe_msg["method"] = "SUBSCRIBE";
        subscribe_msg["params"] = streams;
        subscribe_msg["id"] = generate_subscription_id();

        std::string msg = subscribe_msg.dump();

        // Send subscription message
        int result = client_.write(msg);
        if (result < 0) {
            INFRA_LOG_ERROR("Failed to send subscription message: {}", msg);
            return false;
        }

        INFRA_LOG_INFO("Subscription message sent successfully: {}", msg);

        // Update URI with new streams
        update_streams_uri(symbols);

        // Store subscribed instruments
        subscribed_instruments_.insert(subscribed_instruments_.end(), instrument_keys.begin(), instrument_keys.end());

        INFRA_LOG_INFO("Successfully subscribed to {} instruments with {} streams", symbols.size(), streams.size());

        return true;

    } catch (const std::exception& e) {
        INFRA_LOG_ERROR("Exception during subscription: {}", e.what());
        return false;
    }
}

bool BinanceData::unsubscribe(const std::vector<InstrumentKey>& instrument_keys) {
    try {
        if (instrument_keys.empty()) {
            INFRA_LOG_WARN("No instruments provided for unsubscription");
            return false;
        }

        if (!is_connected_) {
            INFRA_LOG_ERROR("Cannot unsubscribe: WebSocket not connected");
            return false;
        }

        INFRA_LOG_INFO("Unsubscribing from {} instruments", instrument_keys.size());

        // Build unsubscription message
        std::vector<std::string> streams;
        std::vector<std::string> symbols_to_remove;

        for (const auto& key : instrument_keys) {
            std::string symbol = key.instrument_id.to_string();
            std::transform(symbol.begin(), symbol.end(), symbol.begin(), ::tolower);

            // Add to symbols to remove list
            symbols_to_remove.push_back(symbol);

            // Build streams to unsubscribe
            if (enable_kline_) {
                streams.push_back(symbol + "@kline_" + kline_interval_);
            }

            if (enable_depth_) {
                streams.push_back(symbol + "@depth" + std::to_string(depth_levels_));
            }
        }

        if (streams.empty()) {
            INFRA_LOG_WARN("No streams to unsubscribe");
            return false;
        }

        // Build unsubscription message
        Json::json unsubscribe_msg;
        unsubscribe_msg["method"] = "UNSUBSCRIBE";
        unsubscribe_msg["params"] = streams;
        unsubscribe_msg["id"] = generate_subscription_id();

        std::string msg = unsubscribe_msg.dump();

        // Send unsubscription message
        int result = client_.write(msg);
        if (result < 0) {
            INFRA_LOG_ERROR("Failed to send unsubscription message: {}", msg);
            return false;
        }

        INFRA_LOG_INFO("Unsubscription message sent successfully: {}", msg);

        // Remove instruments from subscribed list
        for (const auto& key : instrument_keys) {
            std::string key_id = key.instrument_id.to_string();
            auto it = std::find_if(
                subscribed_instruments_.begin(), subscribed_instruments_.end(),
                [&key_id](const InstrumentKey& existing) { return existing.instrument_id.to_string() == key_id; });
            if (it != subscribed_instruments_.end()) {
                subscribed_instruments_.erase(it);
            }
        }

        // Update URI if needed
        if (!subscribed_instruments_.empty()) {
            update_streams_uri_from_subscribed();
        } else {
            // No more subscriptions, reset to default
            set_default_streams();
        }

        INFRA_LOG_INFO("Successfully unsubscribed from {} instruments", symbols_to_remove.size());

        return true;

    } catch (const std::exception& e) {
        INFRA_LOG_ERROR("Exception during unsubscription: {}", e.what());
        return false;
    }
}

void BinanceData::on_msg(const std::string& msg) {
    try {
        if (msg.empty()) {
            INFRA_LOG_WARN("Received empty message");
            return;
        }

        Json::json json_data = Json::json::parse(msg);

        if (!validate_json_data(json_data)) {
            INFRA_LOG_WARN("Invalid JSON data received: {}", msg);
            return;
        }

        auto mdtype = get_mdtype(json_data);
        switch (mdtype) {
            case enums::MDType::Kline:
                if (enable_kline_) {
                    process_kline_data(json_data);
                }
                break;

            case enums::MDType::Depth:
                if (enable_depth_) {
                    process_depth_data(json_data);
                }
                break;

            default:
                INFRA_LOG_WARN("Unknown message type received: {}", msg);
                break;
        }

    } catch (const Json::json::parse_error& e) {
        INFRA_LOG_ERROR("JSON parse error: {} - Message: {}", e.what(), msg);
    } catch (const std::exception& e) {
        INFRA_LOG_ERROR("Error processing message: {} - Message: {}", e.what(), msg);
    }
}

enums::MDType BinanceData::get_mdtype(const Json::json& data) const {
    try {
        if (!data.contains("stream")) {
            return enums::MDType::Unknown;
        }

        std::string stream_name = data["stream"].get<std::string>();
        if (stream_name.find("kline") != std::string::npos) {
            return enums::MDType::Kline;
        } else if (stream_name.find("depth") != std::string::npos) {
            return enums::MDType::Depth;
        } else {
            return enums::MDType::Unknown;
        }
    } catch (const std::exception& e) {
        INFRA_LOG_ERROR("Error determining message type: {}", e.what());
        return enums::MDType::Unknown;
    }
}

bool BinanceData::process_kline_data(const Json::json& data) {
    try {
        if (!data.contains("data") || !data["data"].contains("k")) {
            INFRA_LOG_WARN("Invalid kline data structure");
            return false;
        }

        const Json::json& kline_json = data["data"]["k"];

        // Check if kline is closed
        if (!kline_json.contains("x") || !kline_json["x"].get<bool>()) {
            return false; // Kline not closed yet
        }

        Bar bar;
        bar.instrument_id = data["data"]["s"].get<std::string>().c_str();
        bar.exchange_id = "binance";
        bar.start_time = kline_json["t"];
        bar.end_time = kline_json["T"];
        bar.open = safe_string_to_double(kline_json["o"].get<std::string>());
        bar.close = safe_string_to_double(kline_json["c"].get<std::string>());
        bar.high = safe_string_to_double(kline_json["h"].get<std::string>());
        bar.low = safe_string_to_double(kline_json["l"].get<std::string>());
        bar.volume = safe_string_to_double(kline_json["v"].get<std::string>());

        auto now_time = infra::time::now_time();
        if (writer_) {
            writer_->write(now_time, bar);
        }

        return true;

    } catch (const std::exception& e) {
        INFRA_LOG_ERROR("Error processing kline data: {}", e.what());
        return false;
    }
}

bool BinanceData::process_depth_data(const Json::json& data) {
    try {
        if (!data.contains("data") || !data["data"].contains("bids") || !data["data"].contains("asks")) {
            INFRA_LOG_WARN("Invalid depth data structure");
            return false;
        }

        Quote depth;
        std::string stream = data["stream"].get<std::string>();
        stream = stream.substr(0, stream.find('@'));
        depth.instrument_id = stream.c_str();
        depth.exchange_id = "binance";

        // Process bids
        size_t bid_idx = 0;
        for (const auto& item : data["data"]["bids"]) {
            if (bid_idx >= depth.bid_price.length) break;

            if (item.is_array() && item.size() >= 2) {
                depth.bid_price[bid_idx] = safe_string_to_double(item[0].get<std::string>());
                depth.bid_volume[bid_idx] = safe_string_to_double(item[1].get<std::string>());
                bid_idx++;
            }
        }
        depth.real_depth_size = bid_idx; /* Setup real depth size */

        // Process asks
        size_t ask_idx = 0;
        for (const auto& item : data["data"]["asks"]) {
            if (ask_idx >= depth.ask_price.length) break;

            if (item.is_array() && item.size() >= 2) {
                depth.ask_price[ask_idx] = safe_string_to_double(item[0].get<std::string>());
                depth.ask_volume[ask_idx] = safe_string_to_double(item[1].get<std::string>());
                ask_idx++;
            }
        }

        auto now_time = infra::time::now_time();
        if (writer_) {
            writer_->write(now_time, depth);
        }

        return true;

    } catch (const std::exception& e) {
        INFRA_LOG_ERROR("Error processing depth data: {}", e.what());
        return false;
    }
}

bool BinanceData::validate_json_data(const Json::json& data) const {
    return data.contains("stream") && data.contains("data");
}

double BinanceData::safe_string_to_double(const std::string& str, double default_value) const {
    try {
        if (str.empty()) return default_value;
        return std::stod(str);
    } catch (const std::exception& e) {
        INFRA_LOG_WARN("Failed to convert string to double: '{}', using default: {}", str, default_value);
        return default_value;
    }
}

int64_t BinanceData::safe_string_to_int64(const std::string& str, int64_t default_value) const {
    try {
        if (str.empty()) return default_value;
        return std::stoll(str);
    } catch (const std::exception& e) {
        INFRA_LOG_WARN("Failed to convert string to int64: '{}', using default: {}", str, default_value);
        return default_value;
    }
}

void BinanceData::on_connect() {
    INFRA_LOG_INFO("Connected to Binance WebSocket server");
    is_connected_ = true;
    should_reconnect_ = false;
    current_reconnect_attempts_ = 0;
}

void BinanceData::on_disconnect() {
    INFRA_LOG_INFO("Disconnected from Binance WebSocket server");
    is_connected_ = false;

    if (should_reconnect_) {
        INFRA_LOG_INFO("Scheduling reconnection...");
        std::thread([this]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(reconnect_interval_ms_));
            if (should_reconnect_) {
                reconnect();
            }
        }).detach();
    }
}

void BinanceData::on_error(const std::string& error) {
    INFRA_LOG_ERROR("WebSocket error: {}", error);
    is_connected_ = false;
}

bool BinanceData::reconnect() {
    if (current_reconnect_attempts_ >= max_reconnect_attempts_) {
        INFRA_LOG_ERROR("Max reconnection attempts reached, giving up");
        should_reconnect_ = false;
        return false;
    }

    current_reconnect_attempts_++;
    INFRA_LOG_INFO("Attempting to reconnect (attempt {}/{})", current_reconnect_attempts_, max_reconnect_attempts_);

    if (client_.open(uri_)) {
        INFRA_LOG_ERROR("Reconnection failed");
        return false;
    }

    INFRA_LOG_INFO("Reconnection successful");
    return true;
}

uint64_t BinanceData::generate_subscription_id() { return ++subscription_id_counter_; }

void BinanceData::update_streams_uri(const std::vector<std::string>& symbols) {
    std::vector<std::string> streams;

    for (const auto& symbol : symbols) {
        if (enable_kline_) {
            streams.push_back(symbol + "@kline_" + kline_interval_);
        }

        if (enable_depth_) {
            streams.push_back(symbol + "@depth" + std::to_string(depth_levels_));
        }
    }

    if (!streams.empty()) {
        std::string streams_param;
        for (size_t i = 0; i < streams.size(); ++i) {
            if (i > 0) streams_param += "/";
            streams_param += streams[i];
        }
        uri_ = s_ws_stream_prefix + "/stream?streams=" + streams_param;
        INFRA_LOG_INFO("Updated streams URI: {}", uri_);
    }
}

void BinanceData::update_streams_uri_from_subscribed() {
    std::vector<std::string> symbols;
    for (const auto& key : subscribed_instruments_) {
        std::string symbol = key.instrument_id.to_string();
        std::transform(symbol.begin(), symbol.end(), symbol.begin(), ::tolower);
        symbols.push_back(symbol);
    }
    update_streams_uri(symbols);
}

void BinanceData::set_default_streams() {
    std::vector<std::string> streams;

    if (enable_kline_) {
        streams.push_back(default_symbol_ + "@kline_" + kline_interval_);
    }

    if (enable_depth_) {
        streams.push_back(default_symbol_ + "@depth" + std::to_string(depth_levels_));
    }

    if (streams.empty()) {
        // Fallback to default configuration
        uri_ = s_ws_stream_prefix + "/stream?streams=btcusdt@kline_1s/btcusdt@depth20";
    } else {
        std::string streams_param;
        for (size_t i = 0; i < streams.size(); ++i) {
            if (i > 0) streams_param += "/";
            streams_param += streams[i];
        }
        uri_ = s_ws_stream_prefix + "/stream?streams=" + streams_param;
    }
}

// Additional helper methods for subscription management
std::vector<std::string> BinanceData::get_subscribed_symbols() const {
    std::vector<std::string> symbols;
    for (const auto& key : subscribed_instruments_) {
        std::string symbol = key.instrument_id.to_string();
        std::transform(symbol.begin(), symbol.end(), symbol.begin(), ::tolower);
        symbols.push_back(symbol);
    }
    return symbols;
}

bool BinanceData::is_subscribed(const std::string& symbol) const {
    std::string lower_symbol = symbol;
    std::transform(lower_symbol.begin(), lower_symbol.end(), lower_symbol.begin(), ::tolower);

    for (const auto& key : subscribed_instruments_) {
        std::string key_symbol = key.instrument_id.to_string();
        std::transform(key_symbol.begin(), key_symbol.end(), key_symbol.begin(), ::tolower);
        if (key_symbol == lower_symbol) {
            return true;
        }
    }
    return false;
}

void BinanceData::clear_subscriptions() {
    if (!subscribed_instruments_.empty()) {
        INFRA_LOG_INFO("Clearing all subscriptions");

        // Send unsubscribe message for all current streams
        std::vector<std::string> current_streams;
        for (const auto& symbol : get_subscribed_symbols()) {
            if (enable_kline_) {
                current_streams.push_back(symbol + "@kline_" + kline_interval_);
            }
            if (enable_depth_) {
                current_streams.push_back(symbol + "@depth" + std::to_string(depth_levels_));
            }
        }

        if (!current_streams.empty()) {
            Json::json unsubscribe_msg;
            unsubscribe_msg["method"] = "UNSUBSCRIBE";
            unsubscribe_msg["params"] = current_streams;
            unsubscribe_msg["id"] = generate_subscription_id();

            std::string msg = unsubscribe_msg.dump();
            client_.write(msg);
            INFRA_LOG_INFO("Sent clear subscriptions message: {}", msg);
        }

        subscribed_instruments_.clear();
        set_default_streams();
    }
}

} // namespace btra::broker