#include <stdio.h>

#include <iostream>

#include "infra/common.h"
#include "infra/format.h"
#include "infra/websocket_client.h"

#include "broker/binance_data.h"

class BinanceMDTest {
   public:
    void test_get_md() {
        infra::WebSocketClient ws;
        ws.set_msg_handler([](const std::string &msg) {
            // Json::json jsondata = Json::json::parse(msg);
            std::cout << msg << std::endl;
        });

        // const std::string uri = "wss://stream.binance.com:443/stream?streams=btcusdt@trade/btcusdt@depth20@100ms";
        const std::string uri = "wss://stream.binance.com:443/stream?streams=btcusdt@kline_1s";
        if (ws.connect(uri) != 0) {
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
            if (!ws.is_connected()) break;
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
};

int main() {
    BinanceMDTest test;
    // test.test_get_md();
    test.test_BinanceData();
    return 0;
}
