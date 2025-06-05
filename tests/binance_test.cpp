#include <stdio.h>

#include <iostream>
#include <fstream>

#include "infra/common.h"
#include "infra/format.h"
#include "infra/websocket_client.h"
#include "infra/time.h"

#include "broker/binance/binance_data.h"
#include "broker/binance/binance_broker.h"

class BinanceTest {
public:
    void test_get_md() {
        infra::WebSocketClient ws;
        ws.set_msg_handler([](const std::string &msg) {
            // Json::json jsondata = Json::json::parse(msg);
            std::cout << msg << std::endl;
        });

        // const std::string uri = "wss://stream.binance.com:443/stream?streams=btcusdt@trade/btcusdt@depth20@100ms";
        const std::string uri = "wss://stream.binance.com:443/stream?streams=btcusdt@kline_1s";
        if (ws.open(uri) != 0) {
            std::cout << "websocket is not connected!" << std::endl;
            return;
        }

        /* Subscribe btc data */
        // {
        //     Json::json jsondata = {
        //         {"method", "SUBSCRIBE"}, {"params", {"btcusdt@aggTrade", "btcusdt@depth"}}, {"id", 1}};
        //     if (ws.write(jsondata.dump())) {
        //         std::cout << "websocket write failed!" << std::endl;
        //         return;
        //     }
        // }

        std::string str;
        while (std::getline(std::cin, str)) {
            if (!ws.is_connected())
                break;
            if (str == "quit") {
                ws.close();
                break;
            }
        }
    }

    void test_BinanceData() {
        btra::broker::BinanceData bd_broker;
        bd_broker.setup(Json::json());
        bd_broker.start();
        std::string str;
        while (std::getline(std::cin, str)) {
            if (str == "quit") {
                break;
            }
        }
    }

    void test_trade() {
        // std::string name = "BTCUSDT";

        // // 原始字符串模板（注意外层大括号转义为 {{ 和 }}）
        // std::string json = fmt::format(R"({{
        //     "id": "e2a85d9f-07a5-4f94-8d5f-789dc3deb097",
        //     "method": "order.place",
        //     "params": {{
        //         "symbol": "{}",
        //         "side": "BUY",
        //         "type": "LIMIT",
        //         "price": "0.1",
        //         "quantity": "10",
        //         "timeInForce": "GTC",
        //         "timestamp": 1655716096498,
        //         "apiKey": "T59MTDLWlpRW16JVeZ2Nju5A5C98WkMm8CSzWC4oqynUlTm1zXOxyauT8LmwXEv9",
        //         "signature": "5942ad337e6779f2f4c62cd1c26dba71c91514400a24990a3e7f5edec9323f90"
        //     }}
        // }})", name);

        // fmt::print("生成的 JSON:\n{}\n", json);

        Json::json jsondata;
        jsondata["id"] = "187d3cb2-942d-484c-8271-4e2141bbadb1";
        jsondata["method"] = "time";
        std::cout << jsondata.dump() << std::endl;

        infra::WebSocketClient ws;
        ws.set_msg_handler([](const std::string &msg) {
            // Json::json jsondata = Json::json::parse(msg);
            std::cout << msg << std::endl;
        });

        const std::string uri = "wss://ws-api.binance.com:443/ws-api/v3";
        if (ws.open(uri) != 0) {
            std::cout << "websocket is not connected!" << std::endl;
            return;
        }
        while (not ws.is_connected()) {}
        ws.write(jsondata.dump());

        std::string str;
        while (std::getline(std::cin, str)) {
            if (str == "quit") {
                break;
            }
        }
    }

    void test_BinanceTrade() {
        btra::broker::BinanceBroker broker;
        std::ifstream f("/home/qiubinglin/btrader/usrconf/binance.json");
        Json::json broker_cfg = Json::json::parse(f);

        broker.setup(broker_cfg);
        broker.start();

        btra::OrderInput order;
        order.order_id = 1;
        order.instrument_id = "ADAUSDT";
        order.side = btra::enums::Side::Buy;
        order.price_type = btra::enums::PriceType::Limit;
        order.volume = 50;
        order.limit_price = 0.4;
        order.time_condition = btra::enums::TimeCondition::GTC;
        order.insert_time = infra::time::now_in_mili();
        broker.insert_order(order);

        std::string str;
        while (std::getline(std::cin, str)) {
            if (str == "quit") {
                break;
            }
        }
    }

    void test_cancel_order() {}
    void test_req_account() {
        btra::broker::BinanceBroker broker;
        std::ifstream f("/home/qiubinglin/btrader/usrconf/binance.json");
        Json::json broker_cfg = Json::json::parse(f);

        broker.setup(broker_cfg);
        broker.start();

        /* Request account status */
        {
            btra::AccountReq req;
            req.type = btra::AccountReq::Status;
            req.id = 0;
            req.insert_time = infra::time::now_in_mili();
            broker.req_account_info(req);
        }

        /* Request account order book */
        {
            btra::AccountReq req;
            req.type = btra::AccountReq::OrderBook;
            req.id = 1;
            req.insert_time = infra::time::now_in_mili();
            broker.req_account_info(req);
        }

        /* Request specified order */
        {
            btra::AccountReq req;
            req.type = btra::AccountReq::Order;
            req.id = 2;
            req.target_id = 7094878517;
            req.instrument_id = "ADAUSDT";
            req.insert_time = infra::time::now_in_mili();
            broker.req_account_info(req);
        }
        std::string str;
        while (std::getline(std::cin, str)) {
            if (str == "quit") {
                break;
            }
        }
    }
};

int main() {
    BinanceTest test;
    // test.test_get_md();
    // test.test_BinanceData();
    // test.test_trade();
    // test.test_BinanceTrade();
    test.test_req_account();
    return 0;
}
