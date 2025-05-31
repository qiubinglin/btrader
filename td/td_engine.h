#pragma once

#include "eventengine.h"
#include "trade_service.h"

namespace btra {

class TDEngine : public EventEngine {
public:
    void react() override;
    void on_setup() override;
    std::string name() const override { return "td"; }

private:
    void on_trading_start(const EventSPtr &event);
    void insert_order(const EventSPtr &event);
    void cancel_order(const EventSPtr &event);

    std::unordered_map<uint32_t, broker::TradeServiceUPtr> trade_services_;
};

} // namespace btra