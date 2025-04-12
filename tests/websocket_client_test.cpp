#include "infra/websocket_client.h"

int main(int argc, char** argv) {
    infra::WebSocketClient ws;
    ws.set_msg_handler([](const std::string& msg) {
        printf("onmessage: %.*s\n", (int)msg.size(), msg.data());
    });

    ws.open("ws://127.0.0.1:9999/test");

    std::string str;
    while (std::getline(std::cin, str)) {
        if (!ws.is_connected()) break;
        if (str == "quit") {
            ws.close();
            break;
        }
        ws.write(str);
    }

    return 0;
}