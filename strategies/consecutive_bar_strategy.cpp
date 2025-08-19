#include "consecutive_bar_strategy.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>

#include "core/types.h"
#include "strategy/executor.h"

namespace btra::strategy {

ConsecutiveBarStrategy::ConsecutiveBarStrategy()
    : last_trade_time_(0), total_pnl_(0.0), max_drawdown_(0.0), peak_equity_(0.0) {
    // Initialize configuration with default values
    config_.min_bars = 3;
    config_.max_position = 1000.0;
    config_.stop_loss = 0.02;
    config_.take_profit = 0.03;
    config_.risk_per_trade = 0.01;
    config_.max_drawdown = 0.10;
    config_.enable_quote_trading = false;

    // Initialize state
    state_.position_opened = false;
    state_.entry_price = 0.0;
    state_.current_position = 0.0;
    state_.consecutive_up_bars = 0;
    state_.consecutive_down_bars = 0;
    state_.bar_count = 0;
}

void ConsecutiveBarStrategy::setup(const Json::json &cfg) {
    std::cout << "ConsecutiveBarStrategy: Setting up strategy configuration..." << std::endl;

    try {
        // Parse basic strategy parameters
        if (cfg.contains("symbol")) {
            symbol_ = cfg["symbol"].get<std::string>();
            std::cout << "ConsecutiveBarStrategy: Symbol set to: " << symbol_ << std::endl;
        }

        if (cfg.contains("institution")) {
            institution_ = cfg["institution"].get<std::string>();
            std::cout << "ConsecutiveBarStrategy: Institution set to: " << institution_ << std::endl;
        }

        if (cfg.contains("account")) {
            account_ = cfg["account"].get<std::string>();
            std::cout << "ConsecutiveBarStrategy: Account set to: " << account_ << std::endl;
        }

        // Parse strategy-specific configuration parameters
        if (cfg.contains("min_bars")) {
            config_.min_bars = cfg["min_bars"].get<int>();
            if (config_.min_bars < 3) {
                std::cout << "ConsecutiveBarStrategy: Warning - min_bars value " << config_.min_bars 
                          << " should be at least 3, using default value 3" << std::endl;
                config_.min_bars = 3;
            }
            std::cout << "ConsecutiveBarStrategy: Minimum bars before trading set to: " << config_.min_bars << std::endl;
        }

        if (cfg.contains("max_position")) {
            config_.max_position = cfg["max_position"].get<double>();
            if (config_.max_position <= 0.0) {
                std::cout << "ConsecutiveBarStrategy: Warning - max_position value " << config_.max_position 
                          << " must be positive, using default value 1000.0" << std::endl;
                config_.max_position = 1000.0;
            }
            std::cout << "ConsecutiveBarStrategy: Maximum position size set to: " << config_.max_position << std::endl;
        }

        if (cfg.contains("stop_loss")) {
            config_.stop_loss = cfg["stop_loss"].get<double>();
            if (config_.stop_loss <= 0.0 || config_.stop_loss > 0.5) {
                std::cout << "ConsecutiveBarStrategy: Warning - stop_loss value " << config_.stop_loss 
                          << " should be between 0.0 and 0.5, using default value 0.02" << std::endl;
                config_.stop_loss = 0.02;
            }
            std::cout << "ConsecutiveBarStrategy: Stop loss percentage set to: " << (config_.stop_loss * 100) << "%" << std::endl;
        }

        if (cfg.contains("take_profit")) {
            config_.take_profit = cfg["take_profit"].get<double>();
            if (config_.take_profit <= 0.0 || config_.take_profit > 0.5) {
                std::cout << "ConsecutiveBarStrategy: Warning - take_profit value " << config_.take_profit 
                          << " should be between 0.0 and 0.5, using default value 0.03" << std::endl;
                config_.take_profit = 0.03;
            }
            std::cout << "ConsecutiveBarStrategy: Take profit percentage set to: " << (config_.take_profit * 100) << "%" << std::endl;
        }

        if (cfg.contains("risk_per_trade")) {
            config_.risk_per_trade = cfg["risk_per_trade"].get<double>();
            if (config_.risk_per_trade <= 0.0 || config_.risk_per_trade > 0.1) {
                std::cout << "ConsecutiveBarStrategy: Warning - risk_per_trade value " << config_.risk_per_trade 
                          << " should be between 0.0 and 0.1, using default value 0.01" << std::endl;
                config_.risk_per_trade = 0.01;
            }
            std::cout << "ConsecutiveBarStrategy: Risk per trade percentage set to: " << (config_.risk_per_trade * 100) << "%" << std::endl;
        }

        if (cfg.contains("max_drawdown")) {
            config_.max_drawdown = cfg["max_drawdown"].get<double>();
            if (config_.max_drawdown <= 0.0 || config_.max_drawdown > 0.5) {
                std::cout << "ConsecutiveBarStrategy: Warning - max_drawdown value " << config_.max_drawdown 
                          << " should be between 0.0 and 0.5, using default value 0.10" << std::endl;
                config_.max_drawdown = 0.10;
            }
            std::cout << "ConsecutiveBarStrategy: Maximum drawdown percentage set to: " << (config_.max_drawdown * 100) << "%" << std::endl;
        }

        if (cfg.contains("enable_quote_trading")) {
            config_.enable_quote_trading = cfg["enable_quote_trading"].get<bool>();
            std::cout << "ConsecutiveBarStrategy: Quote trading " 
                      << (config_.enable_quote_trading ? "enabled" : "disabled") << std::endl;
        }

        // Validate configuration consistency
        if (config_.stop_loss >= config_.take_profit) {
            std::cout << "ConsecutiveBarStrategy: Warning - stop_loss (" << (config_.stop_loss * 100) 
                      << "%) should be less than take_profit (" << (config_.take_profit * 100) 
                      << "%), adjusting stop_loss to 80% of take_profit" << std::endl;
            config_.stop_loss = config_.take_profit * 0.8;
        }

        std::cout << "ConsecutiveBarStrategy: Configuration setup completed successfully" << std::endl;

    } catch (const std::exception &e) {
        std::cout << "ConsecutiveBarStrategy: Error parsing configuration: " << e.what() << std::endl;
        std::cout << "ConsecutiveBarStrategy: Using default configuration values" << std::endl;
    }

    // Print final configuration summary
    std::cout << "\n=== Consecutive Bar Strategy Configuration ===" << std::endl;
    std::cout << "Symbol: " << symbol_ << std::endl;
    std::cout << "Institution: " << institution_ << std::endl;
    std::cout << "Account: " << account_ << std::endl;
    std::cout << "Minimum bars: " << config_.min_bars << std::endl;
    std::cout << "Max position size: " << config_.max_position << std::endl;
    std::cout << "Stop loss: " << (config_.stop_loss * 100) << "%" << std::endl;
    std::cout << "Take profit: " << (config_.take_profit * 100) << "%" << std::endl;
    std::cout << "Risk per trade: " << (config_.risk_per_trade * 100) << "%" << std::endl;
    std::cout << "Max drawdown: " << (config_.max_drawdown * 100) << "%" << std::endl;
    std::cout << "Quote trading: " << (config_.enable_quote_trading ? "enabled" : "disabled") << std::endl;
    std::cout << "=============================================" << std::endl;
}

void ConsecutiveBarStrategy::pre_start(ExecutorSPtr &executor) {
    std::cout << "ConsecutiveBarStrategy: Starting strategy..." << std::endl;

    // Reset state
    state_.bar_count = 0;
    state_.position_opened = false;
    state_.current_position = 0.0;
    state_.consecutive_up_bars = 0;
    state_.consecutive_down_bars = 0;
    state_.bar_history.clear();
    total_pnl_ = 0.0;
    max_drawdown_ = 0.0;
    peak_equity_ = 0.0;
}

void ConsecutiveBarStrategy::post_start(ExecutorSPtr &executor) {
    std::cout << "ConsecutiveBarStrategy: Strategy started successfully" << std::endl;
}

void ConsecutiveBarStrategy::pre_stop(ExecutorSPtr &executor) {
    std::cout << "ConsecutiveBarStrategy: Stopping strategy..." << std::endl;

    // Close any open positions
    if (state_.position_opened && state_.current_position != 0.0) {
        close_all_positions(executor);
    }

    // Print final statistics
    print_performance_summary();
}

void ConsecutiveBarStrategy::post_stop(ExecutorSPtr &executor) {
    std::cout << "ConsecutiveBarStrategy: Strategy stopped" << std::endl;
}

void ConsecutiveBarStrategy::on_quote(ExecutorSPtr &executor, const Quote &quote, JID source) {
    if (!config_.enable_quote_trading) {
        return; // Only trade on bars unless explicitly enabled
    }

    // Use mid price for quote-based trading
    double mid_price = 0.0;
    if (quote.bid_price.size() > 0 && quote.ask_price.size() > 0) {
        mid_price = (quote.bid_price[0] + quote.ask_price[0]) / 2.0;
    } else {
        mid_price = quote.last_price;
    }

    // Check exit conditions
    check_exit_conditions(executor, mid_price);
}

void ConsecutiveBarStrategy::on_bar(ExecutorSPtr &executor, const Bar &bar, JID source) {
    // Update bar history and analyze consecutive bars
    update_bar_history(bar);
    analyze_consecutive_bars();

    // Increment bar counter
    state_.bar_count++;

    // Check exit conditions
    check_exit_conditions(executor, bar.close);

    // Execute trades based on consecutive bar analysis
    if (state_.bar_count >= config_.min_bars) {
        auto current_time =
            std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
                .count();

        if (current_time - last_trade_time_ > MIN_TRADE_INTERVAL) {
            if (should_buy()) {
                execute_trade(executor, bar.close, true);
                last_trade_time_ = current_time;
            } else if (should_sell()) {
                execute_trade(executor, bar.close, false);
                last_trade_time_ = current_time;
            }
        }
    }
}

void ConsecutiveBarStrategy::on_order(ExecutorSPtr &executor, const Order &order, JID source) {
    // Handle order updates
    std::cout << "ConsecutiveBarStrategy: Order update - ID: " << order.order_id
              << ", Status: " << static_cast<int>(order.status) << ", Filled: " << (order.volume - order.volume_left)
              << ", Remaining: " << order.volume_left << std::endl;

    // Store order for tracking
    active_orders_[order.order_id] = order;

    // Remove completed orders
    if (order.status == enums::OrderStatus::Filled || order.status == enums::OrderStatus::Cancelled ||
        order.status == enums::OrderStatus::Error) {
        active_orders_.erase(order.order_id);
    }

    // Log order statistics
    log_order_statistics();
}

void ConsecutiveBarStrategy::on_trade(ExecutorSPtr &executor, const Trade &trade, JID source) {
    // Handle trade execution
    std::cout << "ConsecutiveBarStrategy: Trade executed - ID: " << trade.trade_id << ", Price: " << trade.price
              << ", Volume: " << trade.volume << std::endl;

    // Update position based on trade side
    if (trade.side == enums::Side::Buy) {
        state_.current_position += trade.volume;
    } else if (trade.side == enums::Side::Sell) {
        state_.current_position -= trade.volume;
    }

    if (state_.current_position != 0.0) {
        state_.position_opened = true;
        if (std::abs(state_.current_position) == trade.volume) {
            state_.entry_price = trade.price;
        }
    } else {
        state_.position_opened = false;
        state_.entry_price = 0.0;
    }

    // Store trade for analysis
    trade_history_.push_back(trade);
    if (trade_history_.size() > MAX_TRADE_HISTORY) {
        trade_history_.pop_front();
    }

    // Update PnL tracking
    update_pnl_tracking(trade);
}

void ConsecutiveBarStrategy::on_position_sync_reset(ExecutorSPtr &executor, const PositionBook &old_book,
                                                   const PositionBook &new_book, JID source) {
    // Sync position information from broker
    std::cout << "ConsecutiveBarStrategy: Position synced from broker" << std::endl;

    // Calculate net position from position book
    state_.current_position = calculate_net_position(new_book);

    if (state_.current_position != 0.0) {
        state_.position_opened = true;
        // Try to find entry price from trade history
        if (!trade_history_.empty()) {
            state_.entry_price = trade_history_.back().price;
        }
    } else {
        state_.position_opened = false;
        state_.entry_price = 0.0;
    }

    std::cout << "ConsecutiveBarStrategy: Synced position: " << state_.current_position
              << " at price: " << state_.entry_price << std::endl;
}

void ConsecutiveBarStrategy::update_bar_history(const Bar &bar) {
    // Store bar in history
    state_.bar_history.push_back(bar);
    if (state_.bar_history.size() > MAX_BAR_HISTORY) {
        state_.bar_history.pop_front();
    }
}

void ConsecutiveBarStrategy::analyze_consecutive_bars() {
    if (state_.bar_history.size() < 2) {
        state_.consecutive_up_bars = 0;
        state_.consecutive_down_bars = 0;
        return;
    }

    // Reset counters
    state_.consecutive_up_bars = 0;
    state_.consecutive_down_bars = 0;

    // Analyze the last two bars
    auto it = state_.bar_history.rbegin();
    Bar current_bar = *it;
    Bar previous_bar = *(++it);

    // Check if current bar is up (close > open)
    if (current_bar.close > current_bar.open) {
        state_.consecutive_up_bars = 1;
        // Check if previous bar was also up
        if (previous_bar.close > previous_bar.open) {
            state_.consecutive_up_bars = 2;
        }
    }
    // Check if current bar is down (close < open)
    else if (current_bar.close < current_bar.open) {
        state_.consecutive_down_bars = 1;
        // Check if previous bar was also down
        if (previous_bar.close < previous_bar.open) {
            state_.consecutive_down_bars = 2;
        }
    }

    std::cout << "ConsecutiveBarStrategy: Bar analysis - Up bars: " << state_.consecutive_up_bars 
              << ", Down bars: " << state_.consecutive_down_bars 
              << ", Current: " << (current_bar.close > current_bar.open ? "UP" : 
                                  current_bar.close < current_bar.open ? "DOWN" : "FLAT")
              << " (O:" << current_bar.open << " C:" << current_bar.close << ")" << std::endl;
}

bool ConsecutiveBarStrategy::should_buy() const {
    // Buy signal: two consecutive up bars and no current position
    return state_.consecutive_up_bars >= 2 && !state_.position_opened && 
           max_drawdown_ <= config_.max_drawdown;
}

bool ConsecutiveBarStrategy::should_sell() const {
    // Sell signal: two consecutive down bars and no current position
    return state_.consecutive_down_bars >= 2 && !state_.position_opened && 
           max_drawdown_ <= config_.max_drawdown;
}

void ConsecutiveBarStrategy::execute_trade(ExecutorSPtr &executor, double price, bool is_buy) {
    // Calculate position size with risk management
    double position_size = calculate_position_size(price);

    if (is_buy) {
        std::cout << "ConsecutiveBarStrategy: Sending BUY order - Price: " << price << ", Size: " << position_size
                  << " (2 consecutive UP bars detected)" << std::endl;

        // Create and send buy order
        OrderInput order_input;
        order_input.order_id = generate_order_id();
        order_input.instrument_id = symbol_.c_str();
        order_input.exchange_id = institution_.c_str();
        order_input.instrument_type = enums::InstrumentType::Stock;
        order_input.limit_price = price;
        order_input.volume = static_cast<VolumeType>(position_size);
        order_input.side = enums::Side::Buy;
        order_input.offset = enums::Offset::Open;
        order_input.price_type = enums::PriceType::Limit;
        order_input.volume_condition = enums::VolumeCondition::Any;
        order_input.time_condition = enums::TimeCondition::GFD;

        uint64_t order_id = executor->insert_order(institution_, account_, order_input);
        std::cout << "ConsecutiveBarStrategy: BUY order sent with ID: " << order_id << std::endl;

    } else {
        std::cout << "ConsecutiveBarStrategy: Sending SELL order - Price: " << price << ", Size: " << position_size
                  << " (2 consecutive DOWN bars detected)" << std::endl;

        // Create and send sell order
        OrderInput order_input;
        order_input.order_id = generate_order_id();
        order_input.instrument_id = symbol_.c_str();
        order_input.exchange_id = institution_.c_str();
        order_input.instrument_type = enums::InstrumentType::Stock;
        order_input.limit_price = price;
        order_input.volume = static_cast<VolumeType>(position_size);
        order_input.side = enums::Side::Sell;
        order_input.offset = enums::Offset::Open;
        order_input.price_type = enums::PriceType::Limit;
        order_input.volume_condition = enums::VolumeCondition::Any;
        order_input.time_condition = enums::TimeCondition::GFD;

        uint64_t order_id = executor->insert_order(institution_, account_, order_input);
        std::cout << "ConsecutiveBarStrategy: SELL order sent with ID: " << order_id << std::endl;
    }
}

void ConsecutiveBarStrategy::check_exit_conditions(ExecutorSPtr &executor, double current_price) {
    if (!state_.position_opened || state_.entry_price == 0.0) {
        return;
    }

    double price_change = (current_price - state_.entry_price) / state_.entry_price;

    // Check stop loss
    if (state_.current_position > 0 && price_change < -config_.stop_loss) {
        std::cout << "ConsecutiveBarStrategy: Stop loss triggered at price " << current_price << std::endl;
        close_position(executor, current_price, false); // Sell to close long position

    } else if (state_.current_position < 0 && price_change > config_.stop_loss) {
        std::cout << "ConsecutiveBarStrategy: Stop loss triggered at price " << current_price << std::endl;
        close_position(executor, current_price, true); // Buy to close short position
    }

    // Check take profit
    if (state_.current_position > 0 && price_change > config_.take_profit) {
        std::cout << "ConsecutiveBarStrategy: Take profit triggered at price " << current_price << std::endl;
        close_position(executor, current_price, false); // Sell to close long position

    } else if (state_.current_position < 0 && price_change < -config_.take_profit) {
        std::cout << "ConsecutiveBarStrategy: Take profit triggered at price " << current_price << std::endl;
        close_position(executor, current_price, true); // Buy to close short position
    }
}

void ConsecutiveBarStrategy::close_position(ExecutorSPtr &executor, double price, bool is_buy) {
    if (state_.current_position == 0.0) {
        return;
    }

    double close_volume = std::abs(state_.current_position);

    OrderInput order_input;
    order_input.order_id = generate_order_id();
    order_input.instrument_id = symbol_.c_str();
    order_input.exchange_id = "DEFAULT_EXCHANGE";
    order_input.instrument_type = enums::InstrumentType::Stock;
    order_input.limit_price = price;
    order_input.volume = static_cast<VolumeType>(close_volume);
    order_input.side = is_buy ? enums::Side::Buy : enums::Side::Sell;
    order_input.offset = enums::Offset::Close;
    order_input.price_type = enums::PriceType::Limit;
    order_input.volume_condition = enums::VolumeCondition::All;
    order_input.time_condition = enums::TimeCondition::IOC; // Immediate or Cancel

    uint64_t order_id = executor->insert_order(institution_, account_, order_input);
    std::cout << "ConsecutiveBarStrategy: Position close order sent with ID: " << order_id << std::endl;
}

void ConsecutiveBarStrategy::close_all_positions(ExecutorSPtr &executor) {
    if (state_.current_position == 0.0) {
        return;
    }

    // Get current market price from last known price
    double current_price = 0.0;
    if (!state_.bar_history.empty()) {
        current_price = state_.bar_history.back().close;
    }

    if (current_price > 0.0) {
        close_position(executor, current_price, state_.current_position < 0);
    }

    // Cancel all active orders
    for (const auto &[order_id, order] : active_orders_) {
        if (order.status == enums::OrderStatus::Pending || order.status == enums::OrderStatus::Submitted) {
            executor->cancel_order(order_id);
            std::cout << "ConsecutiveBarStrategy: Cancelled order " << order_id << std::endl;
        }
    }
}

double ConsecutiveBarStrategy::calculate_position_size(double price) {
    // Get available capital from executor (in real implementation)
    double available_capital = 100000.0; // Placeholder

    // Calculate position size based on risk management
    double risk_amount = available_capital * config_.risk_per_trade;
    double position_size = risk_amount / price;

    // Limit to maximum position size
    if (position_size > config_.max_position) {
        position_size = config_.max_position;
    }

    // Ensure minimum position size
    if (position_size < 1.0) {
        position_size = 1.0;
    }

    return position_size;
}

double ConsecutiveBarStrategy::calculate_net_position(const PositionBook &position_book) {
    double net_position = 0.0;

    // Sum up long positions
    for (const auto &[hash_key, position] : position_book.long_positions) {
        if (position.volume > 0) {
            net_position += position.volume;
        }
    }

    // Sum up short positions (negative)
    for (const auto &[hash_key, position] : position_book.short_positions) {
        if (position.volume > 0) {
            net_position -= position.volume;
        }
    }

    return net_position;
}

void ConsecutiveBarStrategy::update_pnl_tracking(const Trade &trade) {
    // Simple PnL calculation (in real implementation, use position book)
    if (state_.entry_price > 0.0) {
        double pnl = 0.0;
        if (trade.side == enums::Side::Buy) {
            pnl = (trade.price - state_.entry_price) * trade.volume;
        } else {
            pnl = (state_.entry_price - trade.price) * trade.volume;
        }

        total_pnl_ += pnl;

        // Update peak equity and drawdown
        double current_equity = 100000.0 + total_pnl_; // Assuming initial capital
        if (current_equity > peak_equity_) {
            peak_equity_ = current_equity;
        }

        double drawdown = (peak_equity_ - current_equity) / peak_equity_;
        if (drawdown > max_drawdown_) {
            max_drawdown_ = drawdown;
        }
    }
}

void ConsecutiveBarStrategy::log_order_statistics() {
    std::cout << "ConsecutiveBarStrategy: Order Statistics - Active: " << active_orders_.size()
              << ", Total PnL: " << total_pnl_ << ", Max Drawdown: " << (max_drawdown_ * 100) << "%" << std::endl;
}

void ConsecutiveBarStrategy::print_performance_summary() {
    std::cout << "\n=== Consecutive Bar Strategy Performance Summary ===" << std::endl;
    std::cout << "Total PnL: " << total_pnl_ << std::endl;
    std::cout << "Peak Equity: " << peak_equity_ << std::endl;
    std::cout << "Max Drawdown: " << (max_drawdown_ * 100) << "%" << std::endl;
    std::cout << "Total Trades: " << trade_history_.size() << std::endl;
    std::cout << "Final Position: " << state_.current_position << std::endl;
    std::cout << "==================================================" << std::endl;
}

uint64_t ConsecutiveBarStrategy::generate_order_id() {
    static uint64_t next_order_id = 1;
    return next_order_id++;
}

} // namespace btra::strategy

REGISTER_STRATEGY_2(ConsecutiveBarStrategy, btra::strategy::ConsecutiveBarStrategy)
