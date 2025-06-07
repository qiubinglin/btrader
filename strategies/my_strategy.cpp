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
        // std::cout << "Received Quote: " << quote.instrument_id.to_string() << " at " << quote.data_time << std::endl;
        // for (size_t i = 0; i < quote.bid_price.size(); ++i) {
        //     std::cout << "Bid Price[" << i << "]: " << quote.bid_price[i] << ", Volume: " << quote.bid_volume[i]
        //               << std::endl;
        // }
        // for (size_t i = 0; i < quote.ask_price.size(); ++i) {
        //     std::cout << "Ask Price[" << i << "]: " << quote.ask_price[i] << ", Volume: " << quote.ask_volume[i]
        //               << std::endl;
        // }
        // You can add your trading logic here based on the quote data
    }

    void on_bar(ExecutorSPtr &executor, const Bar &bar, JID source) override {
        price_sequence_.push(bar);
        if (price_sequence_.size() > 100) {
            price_sequence_.pop();
        }
        // std::cout << "Received Bar: " << bar.instrument_id.to_string() << " at " << bar.end_time << std::endl;
        // std::cout << "Open: " << bar.open << ", High: " << bar.high << ", Low: " << bar.low << ", Close: " << bar.close
        //           << ", Volume: " << bar.volume << std::endl;
        // You can add your trading logic here based on the bar data
        if (bar_cnt_++ % 5 == 0) {
            // {
            //     btra::AccountReq req;
            //     req.type = btra::AccountReq::Status;
            //     executor->req_account_info(institution_, account_, req);
            // }
            {
                std::cout << "Requesting OrderBook for account: " << account_ << std::endl;
                btra::AccountReq req;
                req.type = btra::AccountReq::OrderBook;
                executor->req_account_info(institution_, account_, req);
            }
        }
    }

    void on_order(ExecutorSPtr &executor, const Order &order, JID source) override {
        // Handle order updates here
        std::cout << "Order Update: " << order.order_id << " for " << order.instrument_id.to_string() << " with status "
                  << static_cast<int>(order.status) << std::endl;
    }

    void on_order_action_error(ExecutorSPtr &executor, const OrderActionError &error, JID source) override {
        // Handle order action errors here
        std::cerr << "Order Action Error: " << error.error_msg.to_string() << " for order ID " << error.order_id
                  << std::endl;
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
