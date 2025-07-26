#include "binance_broker.h"

#include <map>

#include "infra/format.h"
#include "infra/log.h"

namespace btra::broker {

static const std::string s_restful_endpoint = "https://api.binance.com/api/v3";
static const std::string s_wss_endpoint = "wss://ws-api.binance.com:443/ws-api/v3";

/**
 * @brief Determine the request type.
 *
 */
enum class Method : int8_t {
    Place,  /* Create order. */
    Cancel, /* Cancel order. */
};
static const std::unordered_map<Method, std::string> s_methods = {
    {Method::Place, "order.place"},
};

static const std::unordered_map<enums::TimeCondition, std::string> s_timecdn = {
    {enums::TimeCondition::GTC, "GTC"}, {enums::TimeCondition::IOC, "IOC"}, {enums::TimeCondition::FOK, "FOK"}};

static const std::unordered_map<enums::PriceType, std::string> s_pricetype = {{enums::PriceType::Limit, "LIMIT"},
                                                                              {enums::PriceType::Any, "MARKET"}};

static const std::unordered_map<enums::Side, std::string> s_side = {{enums::Side::Buy, "BUY"},
                                                                    {enums::Side::Sell, "SELL"}};

static std::string to_msg(const std::map<std::string, std::string> &params_keys) {
    std::string query;
    for (const auto &[key, val] : params_keys) {
        if (query.empty()) {
            query += (key + '=' + val);
        } else {
            query += '&';
            query += (key + '=' + val);
        }
    }
    return query;
}

static std::string get_signature(infra::crypto::EncrypKey &api_secret, const std::string &msg) {
    infra::crypto::Signer signer;
    signer.init_ctx();
    return signer.sign(api_secret, msg);
}

BinanceBroker::~BinanceBroker() {
    if (client_.is_connected()) {
        client_.close();
    }
}

void BinanceBroker::setup(const Json::json &cfg) {
    public_key_str_ = cfg["account"].get<std::string>();
    if (cfg.contains("public_key_file")) {
        auto public_key_file = cfg["public_key_file"].get<std::string>();
        public_key_ = infra::crypto::EncrypKey(public_key_file, false);
    }
    auto secret_key_file = cfg["password"].get<std::string>();
    secret_key_ = infra::crypto::EncrypKey(secret_key_file, true);
    uri_ = s_wss_endpoint;

    client_.on_open([this]() {
        INFRA_LOG_INFO("Binance broker on open!");
        this->state_ = enums::BrokerState::Ready;
    });
    client_.on_close([this]() {
        INFRA_LOG_INFO("Binance broker on close!");
        this->state_ = enums::BrokerState::DisConnected;
    });
    client_.set_msg_handler([this](const std::string &msg) { this->on_msg(msg); });
}

void BinanceBroker::start() {
    if (client_.open(uri_) != 0) {
        INFRA_LOG_ERROR("Connect binance td server failed!");
        throw std::runtime_error("Connect binance td server failed!");
    }
    int try_count = 0;
    while (not client_.is_connected() and try_count < 10) {
        sleep(1);
        ++try_count;
        INFRA_LOG_WARN("Binance broker is trying to connect to server, try count: {}", try_count);
    }
    if (not client_.is_connected()) {
        throw std::runtime_error("Connect binance td server overtime!");
    }
}

void BinanceBroker::stop() {}

enums::AccountType BinanceBroker::get_account_type() const { return enums::AccountType::Stock; }

bool BinanceBroker::insert_order(const OrderInput &input) {
    const char *name = input.instrument_id;
    std::string signature;

    std::map<std::string, std::string> params_keys;
    params_keys["symbol"] = name;
    params_keys["side"] = s_side.at(input.side);
    params_keys["type"] = s_pricetype.at(input.price_type);
    params_keys["price"] = std::to_string(input.limit_price);
    params_keys["quantity"] = std::to_string(input.volume);
    params_keys["timeInForce"] = s_timecdn.at(input.time_condition);
    params_keys["timestamp"] = std::to_string(input.insert_time);
    params_keys["apiKey"] = public_key_str_;

    std::string msg_to_sign = to_msg(params_keys);
    signature = get_signature(secret_key_, msg_to_sign);

    std::string websocket_json = fmt::format(
        R"({{
        "id": "{}",
        "method": "order.place",
        "params": {{
            "symbol": "{}",
            "side": "{}",
            "type": "{}",
            "price": "{}",
            "quantity": "{}",
            "timeInForce": "{}",
            "timestamp": {},
            "apiKey": "{}",
            "signature": "{}"
        }}
    }})",
        input.order_id, params_keys["symbol"], params_keys["side"], params_keys["type"], params_keys["price"],
        params_keys["quantity"], params_keys["timeInForce"], params_keys["timestamp"], public_key_str_, signature);

    // std::cout << websocket_json << std::endl;
    /* Send data. */
    if (client_.write(websocket_json) < 0) {
        std::cerr << "Insert order failed" << std::endl;
    }
    req_book_.add_request(input.order_id, enums::BrokerReqType::OrderPlace);
    return true;
}

bool BinanceBroker::cancel_order(const OrderAction &input) {
    const char *name = input.instrument_id;
    std::string signature;

    std::map<std::string, std::string> params_keys;
    params_keys["symbol"] = name;
    params_keys["origClientOrderId"] = std::to_string(input.target_order_id);
    params_keys["timestamp"] = std::to_string(input.insert_time);
    params_keys["apiKey"] = public_key_str_;

    std::string msg_to_sign = to_msg(params_keys);
    signature = get_signature(secret_key_, msg_to_sign);

    std::string jsondata = fmt::format(R"(
        {{
            "id": "{}",
            "method": "order.cancel",
            "params": {{
                "symbol": "{}",
                "origClientOrderId": "{}",
                "apiKey": "{}",
                "signature": "{}",
                "timestamp": {}
            }}
        }}
    )",
                                       input.order_id, params_keys["symbol"], params_keys["origClientOrderId"],
                                       params_keys["apiKey"], signature, params_keys["timestamp"]);
    /* Send data. */
    if (client_.write(jsondata) < 0) {
        std::cerr << "Cancel order failed" << std::endl;
    }
    req_book_.add_request(input.order_id, enums::BrokerReqType::OrderCancel);
    return true;
}

bool BinanceBroker::req_account_info(const AccountReq &req) {
    switch (req.type) {
        case AccountReq::Status: {
            std::map<std::string, std::string> params_keys;
            params_keys["timestamp"] = std::to_string(req.insert_time);
            params_keys["apiKey"] = public_key_str_;

            std::string msg_to_sign = to_msg(params_keys);
            std::string signature = get_signature(secret_key_, msg_to_sign);

            std::string jsondata = fmt::format(R"(
            {{
                "id": "{}",
                "method": "account.status",
                "params": {{
                    "apiKey": "{}",
                    "signature": "{}",
                    "timestamp": {}
                }}
            }}
        )",
                                               req.id, params_keys["apiKey"], signature, params_keys["timestamp"]);
            /* Send data. */
            if (client_.write(jsondata) < 0) {
                std::cerr << "Req account status failed" << std::endl;
            }
            req_book_.add_request(req.id, enums::BrokerReqType::PositionBook);
        } break;
        case AccountReq::OrderBook: {
            std::map<std::string, std::string> params_keys;
            params_keys["timestamp"] = std::to_string(req.insert_time);
            params_keys["apiKey"] = public_key_str_;

            std::string msg_to_sign = to_msg(params_keys);
            std::string signature = get_signature(secret_key_, msg_to_sign);
            std::string jsondata = fmt::format(R"(
            {{
                "id": "{}",
                "method": "openOrders.status",
                "params": {{
                    "apiKey": "{}",
                    "signature": "{}",
                    "timestamp": {}
                }}
            }}
        )",
                                               req.id, params_keys["apiKey"], signature, params_keys["timestamp"]);
            /* Send data. */
            if (client_.write(jsondata) < 0) {
                std::cerr << "Req account order book failed" << std::endl;
                std::cerr << "Request: " << jsondata << std::endl;
            }
            req_book_.add_request(req.id, enums::BrokerReqType::OrderBook);
        } break;
        case AccountReq::Order: {
            const char *name = req.instrument_id;

            std::map<std::string, std::string> params_keys;
            params_keys["symbol"] = name;
            params_keys["orderId"] = std::to_string(req.target_id);
            params_keys["timestamp"] = std::to_string(req.insert_time);
            params_keys["apiKey"] = public_key_str_;

            std::string msg_to_sign = to_msg(params_keys);
            std::string signature = get_signature(secret_key_, msg_to_sign);
            std::string jsondata = fmt::format(R"(
            {{
                "id": "{}",
                "method": "order.status",
                "params": {{
                    "symbol": "{}",
                    "orderId": {},
                    "apiKey": "{}",
                    "signature": "{}",
                    "timestamp": {}
                }}
            }}
        )",
                                               req.id, params_keys["symbol"], params_keys["orderId"],
                                               params_keys["apiKey"], signature, params_keys["timestamp"]);
            /* Send data. */
            if (client_.write(jsondata) < 0) {
                std::cerr << "Req account status failed" << std::endl;
                std::cerr << "Request: " << jsondata << std::endl;
            }
            req_book_.add_request(req.id, enums::BrokerReqType::OrderState);
        } break;
        case AccountReq::PositionBook:
            break;
        default:
            break;
    }
    return true;
}

void BinanceBroker::on_msg(const std::string &msg) {
    INFRA_LOG_INFO("Binance broker received message: {}", msg);
    // Here you would parse the message and handle it accordingly.
    // This is a placeholder for the actual message handling logic.
    // You might want to deserialize the JSON and process the data.
    Json::json json_msg;
    try {
        json_msg = Json::json::parse(msg);
        if (json_msg.contains("id")) {
            uint64_t req_id = std::stoull(json_msg["id"].get<std::string>());
            if (req_book_.has_request(req_id)) {
                auto req_type = req_book_.get_request(req_id);
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
            } else {
                INFRA_LOG_WARN("Received response for unknown request ID: {}", req_id);
            }
        }
    } catch (const Json::json::parse_error &e) {
        INFRA_LOG_ERROR("Failed to parse JSON message: {}", e.what());
        return;
    }
}

} // namespace btra::broker