// #include <stdio.h>

// #include "infra/websocket_client.h"
// #include "infra/common.h"

// class MDTest {
// public:
//     void test_get_md() {
//         infra::WebSocketClient client;
//         client.set_msg_handler([](const std::string &msg){
            
//         });

//         std::string uri = "wss://ws-api.binance.com:443/ws-api/v3/btcusdt@trade";
//         client.connect(uri);

//         client.run();
//     }
// };

// // int main() {
// //     MDTest test;
// //     test.test_get_md();
// //     return 0;
// // }

// // #include "WebSocketClient.h"
// using namespace hv;

int main(int argc, char** argv) {
//     WebSocketClient ws;
//     ws.onopen = []() {
//         printf("onopen\n");
//     };
//     ws.onmessage = [](const std::string& msg) {
//         printf("onmessage: %.*s\n", (int)msg.size(), msg.data());
//     };
//     ws.onclose = []() {
//         printf("onclose\n");
//     };

//     // reconnect: 1,2,4,8,10,10,10...
//     reconn_setting_t reconn;
//     reconn_setting_init(&reconn);
//     reconn.min_delay = 1000;
//     reconn.max_delay = 10000;
//     reconn.delay_policy = 2;
//     ws.setReconnect(&reconn);

//     ws.open("wss://data-stream.binance.vision:443/ws/btcusdt@kline_1m");

//     std::string str;
//     while (std::getline(std::cin, str)) {
//         if (!ws.isConnected()) break;
//         if (str == "quit") {
//             ws.close();
//             break;
//         }
//         ws.send(str);
//     }

    return 0;
}