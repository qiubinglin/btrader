#include "binance_broker.h"

#include "infra/format.h"

#include <map>

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

static const std::unordered_map<enums::TimeCondition, std::string> s_timecdn = {{enums::TimeCondition::GTC, "GTC"}};

static const std::unordered_map<enums::PriceType, std::string> s_pricetype = {{enums::PriceType::Limit, "LIMIT"}};

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

    client_.set_msg_handler([this](const std::string &msg) { this->on_msg(msg); });
}

void BinanceBroker::start() {
    if (client_.open(uri_) != 0) {
        std::cerr << "Connect binance td server failed!" << std::endl;
        throw std::runtime_error("Connect binance td server failed!");
    }
    int try_count = 0;
    while (not client_.is_connected() and try_count < 100) {
        sleep(1);
        ++try_count;
    }
    if (not client_.is_connected()) {
        throw std::runtime_error("Connect binance td server overtime!");
    }
}

enums::AccountType BinanceBroker::get_account_type() const { return enums::AccountType::Stock; }

bool BinanceBroker::insert_order(const OrderInput &input) {
    /* OrderInput to binance json data. */
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

    std::string websocket_json =
        fmt::format(R"({{
        "id": "{}",
        "method": "{}",
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
                    input.order_id, s_methods.at(Method::Place), params_keys["symbol"], params_keys["side"],
                    params_keys["type"], params_keys["price"], params_keys["quantity"], params_keys["timeInForce"],
                    params_keys["timestamp"], public_key_str_, signature);

    /* Send data. */
    if (client_.write(websocket_json) < 0) {
        std::cerr << "Insert order failed" << std::endl;
    }
    return true;
}

bool BinanceBroker::cancel_order(const OrderAction &input) {
    /* OrderAction to binance json data. */
    Json::json jsondata;
    /* Send data. */
    if (client_.write(jsondata.dump())) {
        std::cerr << "Cancel order failed" << std::endl;
    }
    return true;
}

void BinanceBroker::on_msg(const std::string &msg) { std::cout << msg << std::endl; }

} // namespace btra::broker