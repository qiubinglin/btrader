#include <iostream>

#include "core/book.h"
#include "core/types.h"
#include "strategy/strategy.h"

using namespace btra;
using namespace btra::strategy;

class MyStrategy : public Strategy {
public:
    MyStrategy() { printf("MyStrategy\n"); }

    void on_quote(ExecutorSPtr &executor, const Quote &quote, JID source) override {
        quote_sequence_.push(quote);
        if (quote_sequence_.size() > 100) {
            quote_sequence_.pop();
        }

        for (size_t i = 0; i < quote.bid_price.size(); ++i) {
            INFRA_LOG_INFO("Bid Price[{}]: {}, Volume: {}", i, quote.bid_price[i], quote.bid_volume[i]);
        }
        for (size_t i = 0; i < quote.ask_price.size(); ++i) {
            INFRA_LOG_INFO("Ask Price[{}]: {}, Volume: {}", i, quote.ask_price[i], quote.ask_volume[i]);
        }
        // You can add your trading logic here based on the quote data
    }

    void on_bar(ExecutorSPtr &executor, const Bar &bar, JID source) override {
        price_sequence_.push(bar);
        if (price_sequence_.size() > 100) {
            price_sequence_.pop();
        }
        INFRA_LOG_INFO("Bar Data: {} {} {} {} {}", bar.open, bar.high, bar.low, bar.close, bar.volume);
        // You can add your trading logic here based on the bar data
        if (bar_cnt_++ % 10 == 0) {
            {
                btra::AccountReq req;
                req.type = btra::AccountReq::Status;
                executor->req_account_info(institution_, account_, req);
            }
            {
                btra::AccountReq req;
                req.type = btra::AccountReq::OrderBook;
                executor->req_account_info(institution_, account_, req);
            }
        }
    }

    void on_order(ExecutorSPtr &executor, const Order &order, JID source) override {
        // Handle order updates here
        INFRA_LOG_INFO("Order Update: {} for {} with status {}", order.order_id, order.instrument_id.to_string(),
                       static_cast<int>(order.status));
    }

    void on_order_action_error(ExecutorSPtr &executor, const OrderActionError &error, JID source) override {
        // Handle order action errors here
        INFRA_LOG_INFO("Order Action Error: {} for order ID {}", error.error_msg.to_string(), error.order_id);
    }

    void on_position_sync_reset(ExecutorSPtr &executor, const PositionBookFn &old_book, const PositionBookFn &new_book,
                                JID source) override {}

    void on_asset_sync_reset(ExecutorSPtr &executor, const Asset &old_asset, const Asset &new_asset,
                             JID source) override {}

    void on_broker_state_change(ExecutorSPtr &executor, const BrokerStateUpdate &broker_state_update,
                                JID source) override {}

private:
    std::string institution_{"binance"};
    std::string account_{"euHZeWJLF1IO5MNsuKIQYqJH4ZqLfFbtwxp0ychQi41f7CDBRRCrSt2d7nRzXq1T"};

    std::queue<Bar> price_sequence_;
    std::queue<Quote> quote_sequence_;

    size_t bar_cnt_{0};
};

REGISTER_STRATEGY(MyStrategy)
