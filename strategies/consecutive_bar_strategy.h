#pragma once

#include <deque>
#include <memory>
#include <unordered_map>

#include "core/types.h"
#include "nlohmann/json.hpp"
#include "strategy/strategy.h"

namespace btra::strategy {

/**
 * @brief Consecutive bar strategy
 *
 * This strategy generates trading signals based on consecutive bar movements:
 * - Buy when two consecutive bars show upward movement (close > open)
 * - Sell when two consecutive bars show downward movement (close < open)
 * - Simple and effective for trending markets
 */
class ConsecutiveBarStrategy : public Strategy {
public:
    ConsecutiveBarStrategy();
    virtual ~ConsecutiveBarStrategy() = default;

    void setup(const Json::json &cfg) override;

    // Strategy lifecycle callbacks
    void pre_start(ExecutorSPtr &executor) override;
    void post_start(ExecutorSPtr &executor) override;
    void pre_stop(ExecutorSPtr &executor) override;
    void post_stop(ExecutorSPtr &executor) override;

    // Market data callbacks
    void on_quote(ExecutorSPtr &executor, const Quote &quote, JID source) override;
    void on_bar(ExecutorSPtr &executor, const Bar &bar, JID source) override;

    // Trading callbacks
    void on_order(ExecutorSPtr &executor, const Order &order, JID source) override;
    void on_trade(ExecutorSPtr &executor, const Trade &trade, JID source) override;
    void on_position_sync_reset(ExecutorSPtr &executor, const PositionBook &old_book, const PositionBook &new_book,
                                JID source) override;

private:
    // Configuration parameters
    struct Config {
        int min_bars = 3;             // Minimum bars before trading (including the 2 consecutive bars)
        double max_position = 1000.0; // Maximum position size
        double stop_loss = 0.02;      // Stop loss percentage
        double take_profit = 0.03;    // Take profit percentage
        double risk_per_trade = 0.01; // Risk per trade as percentage of capital
        double max_drawdown = 0.10;   // Maximum drawdown allowed
        bool enable_quote_trading = false; // Whether to trade on quotes or only on bars
    };

    // Strategy state
    struct State {
        std::deque<Bar> bar_history;  // Store recent bars for analysis
        bool position_opened = false;
        double entry_price = 0.0;
        double current_position = 0.0;
        int consecutive_up_bars = 0;   // Count of consecutive up bars
        int consecutive_down_bars = 0; // Count of consecutive down bars
        int bar_count = 0;
    };

    // Private methods
    void update_bar_history(const Bar &bar);
    void analyze_consecutive_bars();
    bool should_buy() const;
    bool should_sell() const;
    void execute_trade(ExecutorSPtr &executor, double price, bool is_buy);
    void check_exit_conditions(ExecutorSPtr &executor, double current_price);
    void close_position(ExecutorSPtr &executor, double price, bool is_buy);
    void close_all_positions(ExecutorSPtr &executor);
    double calculate_position_size(double price);
    double calculate_net_position(const PositionBook &position_book);
    void update_pnl_tracking(const Trade &trade);
    void log_order_statistics();
    void print_performance_summary();
    uint64_t generate_order_id();

    // Member variables
    Config config_;
    State state_;
    std::string symbol_;
    std::string institution_;
    std::string account_;
    int64_t last_trade_time_;

    // Performance tracking
    double total_pnl_;
    double max_drawdown_;
    double peak_equity_;

    // Order and trade tracking
    std::unordered_map<uint64_t, Order> active_orders_;
    std::deque<Trade> trade_history_;

    // Constants
    static constexpr int MAX_BAR_HISTORY = 50;
    static constexpr int MAX_TRADE_HISTORY = 1000;
    static constexpr int64_t MIN_TRADE_INTERVAL = 1000; // 1 second in milliseconds
};

} // namespace btra::strategy
