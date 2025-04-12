#include "websocket_client.h"

namespace infra {

WebSocketClient::WebSocketClient() {
  client_.setPingInterval(0); /* hv::WebSocketClient will auto response pong. */
  client_.onopen = []() {
    printf("onopen\n");
  };

  client_.onclose = []() {
    printf("onclose\n");
  };

  reconn_setting_t reconn;
  reconn_setting_init(&reconn);
  reconn.min_delay = 1000;
  reconn.max_delay = 10000;
  reconn.delay_policy = 2;
  client_.setReconnect(&reconn);
}

WebSocketClient::~WebSocketClient() {}

void WebSocketClient::set_msg_handler(std::function<void(const std::string &)> handler) { client_.onmessage = handler; }

int WebSocketClient::open(const std::string &uri) { return client_.open(uri.c_str()); }

}  // namespace infra