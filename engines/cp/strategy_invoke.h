#pragma once

#include "strategy/executor.h"
#include "strategy/strategy.h"

namespace btra {

struct Invoker {
    template <typename SUBSCRIBER, typename OnMethod = void (strategy::Strategy::*)(strategy::ExecutorSPtr &)>
    static void invoke(SUBSCRIBER &&subscriber, OnMethod method) {
        auto context = subscriber.engine_->executor_;
        for (const auto &strategy : subscriber.engine_->strategies_) {
            (*strategy.*method)(context);
        }
    }

    template <typename SUBSCRIBER, typename TradingData,
              typename OnMethod = void (strategy::Strategy::*)(strategy::ExecutorSPtr &, const TradingData &)>
    static void invoke(SUBSCRIBER &&subscriber, OnMethod method, const TradingData &data) {
        auto context = subscriber.engine_->executor_;
        for (const auto &strategy : subscriber.engine_->strategies_) {
            (*strategy.*method)(context, data);
        }
    }

    template <typename SUBSCRIBER, typename TradingData1, typename TradingData2,
              typename OnMethod = void (strategy::Strategy::*)(strategy::ExecutorSPtr &, const TradingData1 &,
                                                               const TradingData2 &)>
    static void invoke(SUBSCRIBER &&subscriber, OnMethod method, const TradingData1 &data1, const TradingData2 &data2) {
        auto context = subscriber.engine_->executor_;
        for (const auto &strategy : subscriber.engine_->strategies_) {
            (*strategy.*method)(context, data1, data2);
        }
    }

    template <typename SUBSCRIBER, typename TradingData1, typename TradingData2,
              typename OnMethod = void (strategy::Strategy::*)(strategy::ExecutorSPtr &, const TradingData1 &,
                                                               const TradingData2 &, JID)>
    static void invoke(SUBSCRIBER &&subscriber, OnMethod method, const TradingData1 &data1, const TradingData2 &data2,
                       JID source) {
        auto context = subscriber.engine_->executor_;
        for (const auto &strategy : subscriber.engine_->strategies_) {
            (*strategy.*method)(context, data1, data2, source);
        }
    }

    template <typename SUBSCRIBER, typename TradingData,
              typename OnMethod = void (strategy::Strategy::*)(strategy::ExecutorSPtr &, const TradingData &, JID)>
    static void invoke(SUBSCRIBER &&subscriber, OnMethod method, const TradingData &data, JID source) {
        auto context = subscriber.engine_->executor_;
        for (const auto &strategy : subscriber.engine_->strategies_) {
            (*strategy.*method)(context, data, source);
        }
    }

    template <typename SUBSCRIBER, typename OnMethod = void (strategy::Strategy::*)(
                                       strategy::ExecutorSPtr &, uint32_t, const std::vector<uint8_t> &, uint32_t, JID)>
    static void invoke(SUBSCRIBER &&subscriber, OnMethod method, uint32_t msg_type, const std::vector<uint8_t> &data,
                       uint32_t length, JID source) {
        auto context = subscriber.engine_->executor_;
        for (const auto &strategy : subscriber.engine_->strategies_) {
            (*strategy.*method)(context, msg_type, data, length, source);
        }
    }
};

} // namespace btra