#pragma once

#include <hv/WebSocketClient.h>

#include <functional>
#include <string>

namespace infra {

class WebSocketClient {
 public:
  WebSocketClient();
  ~WebSocketClient();

  void set_msg_handler(std::function<void(const std::string &)> handler);

  void connect(const std::string &uri);

  void run();

 private:
  hv::WebSocketClient client_;
  std::function<void(const std::string &)> msg_handler_;
};

}  // namespace infra