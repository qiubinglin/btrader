#pragma once

#include <atomic>
#include <memory>
#include <string>
#include <unordered_map>

#include "broker/trade_service.h"
#include "core/requestbook.h"
#include "infra/crypto/signer.h"
#include "infra/websocket_client.h"

namespace btra::broker {

/**
 * @brief Binance WebSocket trading service
 *
 * Provides trading functionality for Binance exchange via WebSocket API.
 * Supports order placement, cancellation, and account information requests.
 */
class BinanceBroker : public TradeService {
public:
    BinanceBroker();
    ~BinanceBroker() override;

    void setup(const Json::json &cfg) override;
    void start() override;
    void stop() override;
    enums::AccountType get_account_type() const override;

    bool insert_order(const OrderInput &input) override;
    bool cancel_order(const OrderAction &input) override;
    bool req_account_info(const AccountReq &req) override;

    // Additional utility methods
    bool is_connected() const;
    std::string get_connection_status() const;
    void reconnect();

    // Helper methods
    void handle_response(uint64_t req_id, enums::BrokerReqType req_type, const Json::json &response);

private:
    // WebSocket event handlers
    void on_msg(const std::string &msg);
    void on_connect();
    void on_disconnect();
    void on_error(const std::string &error);

    // Order management methods
    bool send_order_request(const std::string &method, std::map<std::string, std::string> &params, uint64_t request_id,
                            enums::BrokerReqType req_type);
    std::map<std::string, std::string> build_order_params(const OrderInput &input);
    std::map<std::string, std::string> build_cancel_params(const OrderAction &input);
    std::map<std::string, std::string> build_account_params(const AccountReq &req);

    // Utility methods
    std::string build_query_string(const std::map<std::string, std::string> &params);
    std::string generate_signature(const std::map<std::string, std::string> &params);
    std::string format_websocket_message(const std::string &method, std::map<std::string, std::string> &params,
                                         uint64_t request_id);

    // Configuration and validation
    bool validate_config(const Json::json &cfg);
    void load_api_credentials(const Json::json &cfg);

    // Connection management
    bool establish_connection();
    void handle_connection_timeout();

private:
    // API credentials
    std::string public_key_str_;
    infra::crypto::EncrypKey public_key_;
    infra::crypto::EncrypKey secret_key_;

    // Connection state
    infra::WebSocketClient client_;
    std::string uri_;
    std::atomic<bool> is_connected_{false};
    std::atomic<bool> should_reconnect_{false};

    // Configuration
    int connection_timeout_seconds_{10};
    int max_reconnect_attempts_{5};
    int reconnect_interval_ms_{5000};
    int current_reconnect_attempts_{0};

    // Request tracking
    RequestBook req_book_;

    // Endpoints
    static const std::string s_restful_endpoint;
    static const std::string s_wss_endpoint;

    // Method mappings
    static const std::unordered_map<enums::TimeCondition, std::string> s_time_conditions;
    static const std::unordered_map<enums::PriceType, std::string> s_price_types;
    static const std::unordered_map<enums::Side, std::string> s_sides;
};

} // namespace btra::broker