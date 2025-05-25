#pragma once

#include "data_service.h"
#include "eventengine.h"

namespace btra {

class MDEngine : public EventEngine {
public:
    void react() override;
    void on_active() override;
    void on_setup() override;

private:
    void on_trading_start(const EventSPtr &event);
    void on_subscribe_data(const EventSPtr &event);

private:
    std::unordered_map<uint32_t, broker::DataServiceUPtr> data_services_;
};

} // namespace btra