#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include <hv/WebSocketServer.h>
#pragma GCC diagnostic pop
using namespace hv;

int main(int argc, char** argv) {
    WebSocketService ws;
    ws.onopen = [](const WebSocketChannelPtr& channel, const HttpRequestPtr& req) {
        printf("onopen: GET %s\n", req->Path().c_str());
    };
    ws.onmessage = [](const WebSocketChannelPtr& channel, const std::string& msg) {
        printf("onmessage: %.*s\n", (int)msg.size(), msg.data());
    };
    ws.onclose = [](const WebSocketChannelPtr& channel) {
        printf("onclose\n");
    };

    WebSocketServer server(&ws);
    server.setPort(9999);
    server.setThreadNum(4);
    server.run();
    return 0;
}