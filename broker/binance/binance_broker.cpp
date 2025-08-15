#include "binance_broker.h"

#include <algorithm>
#include <chrono>
#include <thread>

#include "infra/format.h"
#include "infra/log.h"

namespace btra::broker {

// Static member initialization
const std::string BinanceBroker::s_restful_endpoint = "https://api.binance.com/api/v3";
const std::string BinanceBroker::s_wss_endpoint = "wss://ws-api.binance.com:443/ws-api/v3";

const std::unordered_map<enums::TimeCondition, std::string> BinanceBroker::s_time_conditions = {
    {enums::TimeCondition::GTC, "GTC"},
    {enums::TimeCondition::IOC, "IOC"},
    {enums::TimeCondition::FOK, "FOK"},
    {enums::TimeCondition::GFD, "GFD"}};

const std::unordered_map<enums::PriceType, std::string> BinanceBroker::s_price_types = {
    {enums::PriceType::Limit, "LIMIT"},
    {enums::PriceType::Any, "MARKET"},
    {enums::PriceType::FakBest5, "FAK_BEST5"},
    {enums::PriceType::ForwardBest, "FORWARD_BEST"},
    {enums::PriceType::ReverseBest, "REVERSE_BEST"},
    {enums::PriceType::Fak, "FAK"},
    {enums::PriceType::Fok, "FOK"}};

const std::unordered_map<enums::Side, std::string> BinanceBroker::s_sides = {{enums::Side::Buy, "BUY"},
                                                                             {enums::Side::Sell, "SELL"}};

BinanceBroker::BinanceBroker() {}

BinanceBroker::~BinanceBroker() { stop(); }

void BinanceBroker::setup(const Json::json &cfg) {
    try {
        if (!validate_config(cfg)) {
            throw std::runtime_error("Invalid configuration");
        }

        load_api_credentials(cfg);
        uri_ = s_wss_endpoint;

        INFRA_LOG_INFO("BinanceBroker configured successfully");

        // Initialize WebSocket client with event handlers
        client_.on_open([this]() { this->on_connect(); });
        client_.on_close([this]() { this->on_disconnect(); });
        client_.set_msg_handler([this](const std::string &msg) { this->on_msg(msg); });

    } catch (const std::exception &e) {
        INFRA_LOG_ERROR("Failed to setup BinanceBroker: {}", e.what());
        throw;
    }
}

void BinanceBroker::start() {
    try {
        INFRA_LOG_INFO("Starting Binance broker...");

        if (!establish_connection()) {
            throw std::runtime_error("Failed to establish WebSocket connection");
        }

        INFRA_LOG_INFO("Binance broker started successfully");

    } catch (const std::exception &e) {
        INFRA_LOG_ERROR("Failed to start Binance broker: {}", e.what());
        throw;
    }
}

void BinanceBroker::stop() {
    INFRA_LOG_INFO("Stopping Binance broker...");
    should_reconnect_ = false;
    is_connected_ = false;

    if (client_.is_connected()) {
        client_.close();
    }

    INFRA_LOG_INFO("Binance broker stopped");
}

enums::AccountType BinanceBroker::get_account_type() const { return enums::AccountType::Stock; }

bool BinanceBroker::insert_order(const OrderInput &input) {
    try {
        if (!is_connected_) {
            INFRA_LOG_ERROR("Cannot insert order: broker not connected");
            return false;
        }

        auto params = build_order_params(input);
        return send_order_request("order.place", params, input.order_id, enums::BrokerReqType::OrderPlace);

    } catch (const std::exception &e) {
        INFRA_LOG_ERROR("Exception during order insertion: {}", e.what());
        return false;
    }
}

bool BinanceBroker::cancel_order(const OrderAction &input) {
    try {
        if (!is_connected_) {
            INFRA_LOG_ERROR("Cannot cancel order: broker not connected");
            return false;
        }

        auto params = build_cancel_params(input);
        return send_order_request("order.cancel", params, input.order_id, enums::BrokerReqType::OrderCancel);

    } catch (const std::exception &e) {
        INFRA_LOG_ERROR("Exception during order cancellation: {}", e.what());
        return false;
    }
}

bool BinanceBroker::req_account_info(const AccountReq &req) {
    try {
        if (!is_connected_) {
            INFRA_LOG_ERROR("Cannot request account info: broker not connected");
            return false;
        }

        switch (req.type) {
            case AccountReq::Status: {
                auto params = build_account_params(req);
                return send_order_request("account.status", params, req.id, enums::BrokerReqType::PositionBook);
            } break;

            case AccountReq::OrderBook: {
                auto params = build_account_params(req);
                return send_order_request("openOrders.status", params, req.id, enums::BrokerReqType::OrderBook);
            } break;

            case AccountReq::Order: {
                auto params = build_account_params(req);
                params["symbol"] = req.instrument_id.to_string();
                params["orderId"] = std::to_string(req.target_id);
                return send_order_request("order.status", params, req.id, enums::BrokerReqType::OrderState);
            } break;

            case AccountReq::PositionBook:
                INFRA_LOG_WARN("PositionBook request type not implemented");
                break;

            default:
                INFRA_LOG_WARN("Unknown account request type: {}", static_cast<int>(req.type));
                break;
        }

        return true;

    } catch (const std::exception &e) {
        INFRA_LOG_ERROR("Exception during account info request: {}", e.what());
        return false;
    }
}

void BinanceBroker::on_msg(const std::string &msg) {
    try {
        if (msg.empty()) {
            INFRA_LOG_WARN("Received empty message");
            return;
        }

        INFRA_LOG_INFO("Binance broker received message: {}", msg);

        Json::json json_msg = Json::json::parse(msg);

        if (json_msg.contains("id")) {
            uint64_t req_id = std::stoull(json_msg["id"].get<std::string>());

            if (req_book_.has_request(req_id)) {
                auto req_type = req_book_.get_request(req_id);
                handle_response(req_id, req_type, json_msg);
            } else {
                INFRA_LOG_WARN("Received response for unknown request ID: {}", req_id);
            }
        } else if (json_msg.contains("error")) {
            INFRA_LOG_ERROR("Received error response: {}", json_msg["error"].dump());
        } else {
            INFRA_LOG_INFO("Received message without ID: {}", msg);
        }

    } catch (const Json::json::parse_error &e) {
        INFRA_LOG_ERROR("Failed to parse JSON message: {} - Message: {}", e.what(), msg);
    } catch (const std::exception &e) {
        INFRA_LOG_ERROR("Exception during message processing: {} - Message: {}", e.what(), msg);
    }
}

// WebSocket event handlers
void BinanceBroker::on_connect() {
    INFRA_LOG_INFO("Binance broker WebSocket connected");
    is_connected_ = true;
    should_reconnect_ = false;
    current_reconnect_attempts_ = 0;
    this->state_ = enums::BrokerState::Ready;
}

void BinanceBroker::on_disconnect() {
    INFRA_LOG_INFO("Binance broker WebSocket disconnected");
    is_connected_ = false;
    this->state_ = enums::BrokerState::DisConnected;

    // if (should_reconnect_) {
    //     INFRA_LOG_INFO("Scheduling reconnection...");
    //     std::thread([this]() {
    //         std::this_thread::sleep_for(std::chrono::milliseconds(reconnect_interval_ms_));
    //         if (should_reconnect_) {
    //             reconnect();
    //         }
    //     }).detach();
    // }
}

void BinanceBroker::on_error(const std::string &error) {
    INFRA_LOG_ERROR("Binance broker WebSocket error: {}", error);
    is_connected_ = false;
    this->state_ = enums::BrokerState::LoginFailed;
}

// Order management methods
bool BinanceBroker::send_order_request(const std::string &method, std::map<std::string, std::string> &params,
                                       uint64_t request_id, enums::BrokerReqType req_type) {
    try {
        std::string message = format_websocket_message(method, params, request_id);

        int result = client_.write(message);
        if (result < 0) {
            INFRA_LOG_ERROR("Failed to send {} request: {}", method, message);
            return false;
        }

        req_book_.add_request(request_id, req_type);
        INFRA_LOG_INFO("Successfully sent {} request with ID: {}", method, request_id);
        return true;

    } catch (const std::exception &e) {
        INFRA_LOG_ERROR("Exception during {} request: {}", method, e.what());
        return false;
    }
}

std::map<std::string, std::string> BinanceBroker::build_order_params(const OrderInput &input) {
    std::map<std::string, std::string> params;

    params["symbol"] = input.instrument_id.to_string();
    params["side"] = s_sides.at(input.side);
    params["type"] = s_price_types.at(input.price_type);
    params["quantity"] = std::to_string(input.volume);
    params["timeInForce"] = s_time_conditions.at(input.time_condition);
    params["timestamp"] = std::to_string(input.insert_time);
    params["apiKey"] = public_key_str_;

    if (input.price_type == enums::PriceType::Limit) {
        params["price"] = std::to_string(input.limit_price);
    }

    return params;
}

std::map<std::string, std::string> BinanceBroker::build_cancel_params(const OrderAction &input) {
    std::map<std::string, std::string> params;

    params["symbol"] = input.instrument_id.to_string();
    params["origClientOrderId"] = std::to_string(input.target_order_id);
    params["timestamp"] = std::to_string(input.insert_time);
    params["apiKey"] = public_key_str_;

    return params;
}

std::map<std::string, std::string> BinanceBroker::build_account_params(const AccountReq &req) {
    std::map<std::string, std::string> params;

    params["timestamp"] = std::to_string(req.insert_time);
    params["apiKey"] = public_key_str_;

    return params;
}

// Utility methods
std::string BinanceBroker::build_query_string(const std::map<std::string, std::string> &params) {
    std::string query;
    for (const auto &[key, val] : params) {
        if (!query.empty()) {
            query += '&';
        }
        query += (key + '=' + val);
    }
    return query;
}

std::string BinanceBroker::generate_signature(const std::map<std::string, std::string> &params) {
    std::string query_string = build_query_string(params);

    infra::crypto::Signer signer;
    signer.init_ctx();
    return signer.sign(secret_key_, query_string);
}

std::string BinanceBroker::format_websocket_message(const std::string &method,
                                                    std::map<std::string, std::string> &params, uint64_t request_id) {
    // Generate signature for parameters
    std::string signature = generate_signature(params);
    params["signature"] = signature;

    // Build JSON message
    Json::json message;
    message["id"] = std::to_string(request_id);
    message["method"] = method;
    message["params"] = params;

    return message.dump();
}

// Configuration and validation
bool BinanceBroker::validate_config(const Json::json &cfg) {
    if (!cfg.contains("account")) {
        INFRA_LOG_ERROR("Configuration missing 'account' field");
        return false;
    }

    if (!cfg.contains("password")) {
        INFRA_LOG_ERROR("Configuration missing 'password' field");
        return false;
    }

    return true;
}

void BinanceBroker::load_api_credentials(const Json::json &cfg) {
    public_key_str_ = cfg["account"].get<std::string>();

    if (cfg.contains("public_key_file")) {
        auto public_key_file = cfg["public_key_file"].get<std::string>();
        public_key_ = infra::crypto::EncrypKey(public_key_file, false);
    }

    auto secret_key_file = cfg["password"].get<std::string>();
    secret_key_ = infra::crypto::EncrypKey(secret_key_file, true);

    INFRA_LOG_INFO("API credentials loaded successfully");
}

// Connection management
bool BinanceBroker::establish_connection() {
    try {
        if (client_.open(uri_) != 0) {
            INFRA_LOG_ERROR("Failed to open WebSocket connection to: {}", uri_);
            return false;
        }

        // Wait for connection with timeout
        int try_count = 0;
        while (!is_connected_ && try_count < connection_timeout_seconds_) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            ++try_count;
            INFRA_LOG_WARN("Waiting for WebSocket connection, attempt: {}/{}", try_count, connection_timeout_seconds_);
        }

        if (!is_connected_) {
            INFRA_LOG_ERROR("WebSocket connection timeout after {} seconds", connection_timeout_seconds_);
            return false;
        }

        INFRA_LOG_INFO("WebSocket connection established successfully");
        return true;

    } catch (const std::exception &e) {
        INFRA_LOG_ERROR("Exception during connection establishment: {}", e.what());
        return false;
    }
}

void BinanceBroker::handle_connection_timeout() {
    INFRA_LOG_ERROR("Connection timeout occurred");
    is_connected_ = false;
    should_reconnect_ = true;
}

// Additional utility methods
bool BinanceBroker::is_connected() const { return is_connected_; }

std::string BinanceBroker::get_connection_status() const {
    if (is_connected_) {
        return "Connected";
    } else if (should_reconnect_) {
        return "Reconnecting";
    } else {
        return "Disconnected";
    }
}

void BinanceBroker::reconnect() {
    if (current_reconnect_attempts_ >= max_reconnect_attempts_) {
        INFRA_LOG_ERROR("Max reconnection attempts reached, giving up");
        should_reconnect_ = false;
        return;
    }

    current_reconnect_attempts_++;
    INFRA_LOG_INFO("Attempting to reconnect (attempt {}/{})", current_reconnect_attempts_, max_reconnect_attempts_);

    if (establish_connection()) {
        INFRA_LOG_INFO("Reconnection successful");
        should_reconnect_ = false;
        current_reconnect_attempts_ = 0;
    } else {
        INFRA_LOG_ERROR("Reconnection failed");
    }
}

// Helper method for response handling
void BinanceBroker::handle_response(uint64_t req_id, enums::BrokerReqType req_type, const Json::json &response) {
    switch (req_type) {
        case enums::BrokerReqType::OrderPlace:
            INFRA_LOG_INFO("Order placed successfully, request ID: {}", req_id);
            break;
        case enums::BrokerReqType::OrderCancel:
            INFRA_LOG_INFO("Order cancelled successfully, request ID: {}", req_id);
            break;
        case enums::BrokerReqType::OrderBook:
            INFRA_LOG_INFO("Order book retrieved successfully, request ID: {}", req_id);
            break;
        case enums::BrokerReqType::OrderState:
            INFRA_LOG_INFO("Order state retrieved successfully, request ID: {}", req_id);
            break;
        case enums::BrokerReqType::PositionBook:
            INFRA_LOG_INFO("Position book retrieved successfully, request ID: {}", req_id);
            break;
        default:
            INFRA_LOG_WARN("Unknown request type for ID: {}", req_id);
            break;
    }
}

} // namespace btra::broker