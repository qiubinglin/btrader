#include <iostream>

#include "core/book.h"
#include "core/types.h"
#include "infra/log.h"
#include "strategy/strategy.h"

using namespace btra;
using namespace btra::strategy;

class MyStrategy : public Strategy {
public:
    MyStrategy() {
        printf("MyStrategy\n");
        // Initialize trading parameters
        trading_volume_ = 10.0;
        hold_days_ = 5;
        wait_days_ = 5;
        current_state_ = TradingState::INITIAL_BUY;
        days_counter_ = 0;
        last_action_day_ = 0;

        // Initialize account information (these should be set via configuration)
        institution_ = "default";
        account_ = "default";

        INFRA_LOG_INFO("MyStrategy initialized - Volume: {}, Hold days: {}, Wait days: {}", trading_volume_, hold_days_,
                       wait_days_);
    }

    void setup(const Json::json &cfg) override {
        // Load configuration parameters
        if (cfg.contains("trading_volume")) {
            trading_volume_ = cfg["trading_volume"];
            INFRA_LOG_INFO("Trading volume set to: {}", trading_volume_);
        }

        if (cfg.contains("hold_days")) {
            hold_days_ = cfg["hold_days"];
            INFRA_LOG_INFO("Hold days set to: {}", hold_days_);
        }

        if (cfg.contains("wait_days")) {
            wait_days_ = cfg["wait_days"];
            INFRA_LOG_INFO("Wait days set to: {}", wait_days_);
        }

        if (cfg.contains("institution")) {
            institution_ = cfg["institution"];
            INFRA_LOG_INFO("Institution set to: {}", institution_);
        }

        if (cfg.contains("account")) {
            account_ = cfg["account"];
            INFRA_LOG_INFO("Account set to: {}", account_);
        }

        INFRA_LOG_INFO("Strategy configuration loaded successfully");
    }

    void on_quote(ExecutorSPtr &executor, const Quote &quote, JID source) override {
        quote_sequence_.push(quote);
        if (quote_sequence_.size() > 100) {
            quote_sequence_.pop();
        }
    }

    void on_bar(ExecutorSPtr &executor, const Bar &bar, JID source) override {
        price_sequence_.push(bar);
        if (price_sequence_.size() > 100) {
            price_sequence_.pop();
        }

        execute_trading_logic(executor, bar);
    }

    void on_order(ExecutorSPtr &executor, const Order &order, JID source) override {
        // Handle order updates here
        INFRA_LOG_INFO("Order Update: {} for {} with status {}", order.order_id, order.instrument_id.to_string(),
                       static_cast<int>(order.status));
    }

    void on_order_action_error(ExecutorSPtr &executor, const OrderActionResp &error, JID source) override {
        // Handle order action errors here
        INFRA_LOG_INFO("Order Action Error: {} for order ID {}", error.error_msg.to_string(), error.order_id);
    }

    void on_position_sync_reset(ExecutorSPtr &executor, const PositionBook &old_book, const PositionBook &new_book,
                                JID source) override {}

    void on_asset_sync_reset(ExecutorSPtr &executor, const Asset &old_asset, const Asset &new_asset,
                             JID source) override {}

    void on_broker_state_change(ExecutorSPtr &executor, const BrokerStateUpdate &broker_state_update,
                                JID source) override {}

private:
    // Trading state enumeration
    enum class TradingState {
        INITIAL_BUY, // Initial state, need to buy
        HOLDING,     // Holding position, waiting to sell
        WAITING,     // Sold position, waiting to buy again
        BUYING       // Buying again after waiting period
    };

    // Execute the main trading logic
    void execute_trading_logic(ExecutorSPtr &executor, const Bar &bar) {
        has_position_ = executor->book().positions.has_positions();
        // Check if we need to take action based on current state
        switch (current_state_) {
            case TradingState::INITIAL_BUY:
                if (!has_position_) {
                    place_buy_order(executor, bar);
                    current_state_ = TradingState::HOLDING;
                    days_counter_ = 0;
                    last_action_day_ = bar.end_time;
                    INFRA_LOG_INFO("Initial buy order placed, switching to HOLDING state");
                }
                break;

            case TradingState::HOLDING:
                days_counter_++;
                if (days_counter_ >= hold_days_) {
                    if (has_position_) {
                        place_sell_order(executor, bar);
                        current_state_ = TradingState::WAITING;
                        days_counter_ = 0;
                        last_action_day_ = bar.end_time;
                        INFRA_LOG_INFO("Hold period completed, selling position, switching to WAITING state");
                    } else {
                        // Position was closed unexpectedly, reset to initial state
                        INFRA_LOG_INFO(
                            "Position closed unexpectedly during holding period, resetting to INITIAL_BUY state");
                        current_state_ = TradingState::INITIAL_BUY;
                        days_counter_ = 0;
                    }
                }
                break;

            case TradingState::WAITING:
                days_counter_++;
                if (days_counter_ >= wait_days_) {
                    if (!has_position_) {
                        place_buy_order(executor, bar);
                        current_state_ = TradingState::HOLDING;
                        days_counter_ = 0;
                        last_action_day_ = bar.end_time;
                        INFRA_LOG_INFO("Wait period completed, buying again, switching to HOLDING state");
                    } else {
                        // Position was opened unexpectedly, switch to holding state
                        INFRA_LOG_INFO(
                            "Position opened unexpectedly during waiting period, switching to HOLDING state");
                        current_state_ = TradingState::HOLDING;
                        days_counter_ = 0;
                    }
                }
                break;

            case TradingState::BUYING:
                // This state is not used in current logic, but kept for future expansion
                break;
        }

        // Log current state for debugging
        INFRA_LOG_INFO("Strategy state: {}, Days counter: {}, Has position: {}", static_cast<int>(current_state_),
                       days_counter_, has_position_);
    }

    // Place a buy order
    void place_buy_order(ExecutorSPtr &executor, const Bar &bar) {
        OrderInput order_input;
        order_input.instrument_id = bar.instrument_id;
        order_input.exchange_id = bar.exchange_id;
        order_input.instrument_type = bar.instrument_type;
        order_input.limit_price = bar.close; // Use closing price as limit price
        order_input.volume = trading_volume_;
        order_input.side = enums::Side::Buy;
        order_input.offset = enums::Offset::Open;
        order_input.price_type = enums::PriceType::Limit;
        order_input.volume_condition = enums::VolumeCondition::Any;
        order_input.time_condition = enums::TimeCondition::GFD;
        order_input.hedge_flag = enums::HedgeFlag::Speculation;

        uint64_t order_id = executor->insert_order(institution_, account_, order_input);
        INFRA_LOG_INFO("Buy order placed: ID={}, Volume={}, Price={}", order_id, trading_volume_, bar.close);
    }

    // Place a sell order
    void place_sell_order(ExecutorSPtr &executor, const Bar &bar) {
        OrderInput order_input;
        order_input.instrument_id = bar.instrument_id;
        order_input.exchange_id = bar.exchange_id;
        order_input.instrument_type = bar.instrument_type;
        order_input.limit_price = bar.close; // Use closing price as limit price
        order_input.volume = trading_volume_;
        order_input.side = enums::Side::Sell;
        order_input.offset = enums::Offset::Close;
        order_input.price_type = enums::PriceType::Limit;
        order_input.volume_condition = enums::VolumeCondition::Any;
        order_input.time_condition = enums::TimeCondition::GFD;
        order_input.hedge_flag = enums::HedgeFlag::Speculation;

        uint64_t order_id = executor->insert_order(institution_, account_, order_input);
        INFRA_LOG_INFO("Sell order placed: ID={}, Volume={}, Price={}", order_id, trading_volume_, bar.close);
    }

    std::string institution_;
    std::string account_;

    std::queue<Bar> price_sequence_;
    std::queue<Quote> quote_sequence_;

    size_t bar_cnt_{0};
    bool has_position_{false};

    // Trading strategy parameters
    double trading_volume_;      // Volume to trade (10 units)
    int hold_days_;              // Days to hold position (5 days)
    int wait_days_;              // Days to wait after selling (5 days)
    TradingState current_state_; // Current trading state
    int days_counter_;           // Counter for days in current state
    int64_t last_action_day_;    // Timestamp of last action
};

REGISTER_STRATEGY(MyStrategy)
