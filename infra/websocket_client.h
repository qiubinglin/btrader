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

    /**
     * @brief Send msg to server
     *
     * @param msg
     * @return int means the number of bytes that was successfully send.
     */
    int write(const std::string &msg);

    bool is_connected();
    int close();

private:
    hv::WebSocketClient client_;
    bool is_opened_{false};

    friend class WSClientSetter;
};

class WSClientSetter {
public:
    WSClientSetter(WebSocketClient &wsc) : wsc_(wsc) {}

private:
    WebSocketClient &wsc_;
};

} // namespace infra