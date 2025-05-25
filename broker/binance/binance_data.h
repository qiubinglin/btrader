#pragma once

#include "broker/data_service.h"
#include "infra/websocket_client.h"

namespace btra::broker {

class BinanceData : public DataService {
    static const std::string s_ws_stream_prefix;

public:
    ~BinanceData();
    void setup(const Json::json &cfg) override;
    void start() override;
    void serve() override;

    bool subscribe(const std::vector<InstrumentKey> &instrument_keys) override;
    bool unsubscribe(const std::vector<InstrumentKey> &instrument_keys) override;

private:
    void on_msg(const std::string &msg);

    // infra::WebSocketClient client_;
    std::string uri_;
};

} // namespace btra::broker