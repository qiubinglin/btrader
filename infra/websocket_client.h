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
  int open(const std::string &uri);
  int write(const std::string &msg) { return client_.send(msg); }

  bool is_connected() { return client_.isConnected(); }
  int close() { return client_.close(); }

 private:
  hv::WebSocketClient client_;

  friend class WSClientSetter;
};

class WSClientSetter {
public:
  WSClientSetter(WebSocketClient &wsc) : wsc_(wsc) {}

private:
  WebSocketClient &wsc_;
};

}  // namespace infra