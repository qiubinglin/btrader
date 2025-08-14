#pragma once

#include "broker/data_service.h"
#include "infra/websocket_client.h"
#include <memory>
#include <atomic>
#include <vector>
#include <algorithm>

namespace btra::broker {

/**
 * @brief Binance WebSocket data service
 * 
 * Provides real-time market data from Binance exchange via WebSocket connection.
 * Supports K-line (candlestick) and depth (order book) data streams.
 */
class BinanceData : public DataService {
    static const std::string s_ws_stream_prefix;

public:
    BinanceData();
    ~BinanceData() override;
    
    void setup(const Json::json &cfg) override;
    void start() override;
    void stop() override;

    bool subscribe(const std::vector<InstrumentKey> &instrument_keys) override;
    bool unsubscribe(const std::vector<InstrumentKey> &instrument_keys) override;

private:
    // WebSocket message handling
    void on_msg(const std::string &msg);
    void on_connect();
    void on_disconnect();
    void on_error(const std::string &error);
    
    // Data processing methods
    enums::MDType get_mdtype(const Json::json &data) const;
    bool process_kline_data(const Json::json &data);
    bool process_depth_data(const Json::json &data);
    
    // Utility methods
    bool validate_json_data(const Json::json &data) const;
    double safe_string_to_double(const std::string &str, double default_value = 0.0) const;
    int64_t safe_string_to_int64(const std::string &str, int64_t default_value = 0) const;
    
    // Connection management
    bool reconnect();
    void set_default_streams();
    
    // Subscription management
    uint64_t generate_subscription_id();
    void update_streams_uri(const std::vector<std::string>& symbols);
    void update_streams_uri_from_subscribed();
    
    // Subscription query methods
    std::vector<std::string> get_subscribed_symbols() const;
    bool is_subscribed(const std::string& symbol) const;
    void clear_subscriptions();

private:
    infra::WebSocketClient client_;
    std::string uri_;
    std::string default_symbol_{"btcusdt"};
    std::atomic<bool> is_connected_{false};
    std::atomic<bool> should_reconnect_{false};
    
    // Configuration
    int reconnect_interval_ms_{5000};
    int max_reconnect_attempts_{10};
    int current_reconnect_attempts_{0};
    
    // Stream configuration
    bool enable_kline_{true};
    bool enable_depth_{true};
    std::string kline_interval_{"1s"};
    int depth_levels_{20};
    
    // Subscription tracking
    std::vector<InstrumentKey> subscribed_instruments_;
    uint64_t subscription_id_counter_{1000};
};

} // namespace btra::broker