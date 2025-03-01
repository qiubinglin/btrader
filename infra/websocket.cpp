#include "websocket.h"

namespace infra {

WebSocketClient::WebSocketClient() {
  reconn_setting_t reconn;
  reconn_setting_init(&reconn);
  reconn.min_delay = 1000;
  reconn.max_delay = 10000;
  reconn.delay_policy = 2;
  client_.setReconnect(&reconn);
}

WebSocketClient::~WebSocketClient() {}

void WebSocketClient::set_msg_handler(std::function<void(const std::string &)> handler) { client_.onmessage = handler; }

void WebSocketClient::connect(const std::string &uri) { client_.open(uri.c_str()); }

void WebSocketClient::run() {}

}  // namespace infra