#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include <hv/WebSocketClient.h>
#pragma GCC diagnostic pop

#include <functional>
#include <string>

namespace infra {

class WebSocketClient {
public:
    WebSocketClient();
    ~WebSocketClient();

    void on_open(std::function<void()> handler);
    void on_close(std::function<void()> handler);
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

    friend class WSClientSetter;
};

class WSClientSetter {
public:
    WSClientSetter(WebSocketClient &wsc) : wsc_(wsc) {}

private:
    WebSocketClient &wsc_;
};

} // namespace infra