#include "brokersim.h"

#include <chrono>
#include <iostream>
#include <random>

#include "core/types.h"
#include "infra/time.h"

namespace btra::broker {

BrokerSim::BrokerSim() {
    // 初始化资产
    asset_.update_time = infra::time::now_time();
    asset_.avail = 1000000.0; // 默认100万可用资金
    asset_.margin = 0.0;
    asset_.accumulated_fee = 0.0;

    // 初始化持仓
    positions_ = PositionBook{};
}

void BrokerSim::setup(const Json::json& cfg) {
    Json::json simulation_cfg = cfg["simulation"];
    if (simulation_cfg.contains("commission_rate")) {
        commission_rate_ = simulation_cfg["commission_rate"].get<double>();
    }
    if (simulation_cfg.contains("slippage_rate")) {
        slippage_rate_ = simulation_cfg["slippage_rate"].get<double>();
    }
    if (simulation_cfg.contains("enable_slippage")) {
        enable_slippage_ = simulation_cfg["enable_slippage"].get<bool>();
    }
    if (simulation_cfg.contains("enable_commission")) {
        enable_commission_ = simulation_cfg["enable_commission"].get<bool>();
    }
    if (simulation_cfg.contains("initial_capital")) {
        asset_.avail = simulation_cfg["initial_capital"].get<double>();
    }

    std::cout << "BrokerSim setup completed. Commission rate: " << commission_rate_
              << ", Slippage rate: " << slippage_rate_ << std::endl;
}

void BrokerSim::start() {
    std::lock_guard<std::mutex> lock(mutex_);

    if (running_) {
        return;
    }

    running_ = true;
    state_ = enums::BrokerState::Connected;

    // 启动订单匹配线程
    matching_thread_ = std::thread([this]() {
        while (running_) {
            process_order_matching();
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 100ms间隔
        }
    });

    std::cout << "BrokerSim started successfully" << std::endl;
}

void BrokerSim::stop() {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!running_) {
        return;
    }

    running_ = false;
    state_ = enums::BrokerState::DisConnected;

    if (matching_thread_.joinable()) {
        matching_thread_.join();
    }

    std::cout << "BrokerSim stopped" << std::endl;
}

enums::AccountType BrokerSim::get_account_type() const { return enums::AccountType::BackTest; }

bool BrokerSim::insert_order(const OrderInput& input) {
    std::lock_guard<std::mutex> lock(mutex_);

    try {
        // 创建订单对象
        Order order;
        order_from_input(input, order);

        // 设置订单状态
        order.status = enums::OrderStatus::Submitted;
        order.insert_time = infra::time::now_time();
        order.update_time = infra::time::now_time();

        // 检查资金是否足够
        if (input.side == enums::Side::Buy) {
            double required_cash = input.limit_price * input.volume;
            if (asset_.avail < required_cash) {
                std::cerr << "Insufficient funds for order " << input.order_id << ". Required: " << required_cash
                          << ", Available: " << asset_.avail << std::endl;
                return false;
            }
        }

        // 添加订单到活跃订单列表
        add_order(order);

        std::cout << "Order inserted: " << input.order_id << " " << (input.side == enums::Side::Buy ? "BUY" : "SELL")
                  << " " << input.volume << " @ " << input.limit_price << std::endl;

        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error inserting order: " << e.what() << std::endl;
        return false;
    }
}

bool BrokerSim::cancel_order(const OrderAction& input) {
    std::lock_guard<std::mutex> lock(mutex_);

    try {
        auto it = active_orders_.find(input.target_order_id);
        if (it == active_orders_.end()) {
            std::cerr << "Order " << input.target_order_id << " not found for cancellation" << std::endl;
            return false;
        }

        Order& order = it->second;
        if (order.status != enums::OrderStatus::Pending && order.status != enums::OrderStatus::PartialFilledActive) {
            std::cerr << "Order " << input.target_order_id << " cannot be cancelled in status "
                      << static_cast<int>(order.status) << std::endl;
            return false;
        }

        // 更新订单状态
        update_order_status(input.target_order_id, enums::OrderStatus::Cancelled);

        std::cout << "Order cancelled: " << input.target_order_id << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error cancelling order: " << e.what() << std::endl;
        return false;
    }
}

bool BrokerSim::req_account_info(const AccountReq& req) {
    std::lock_guard<std::mutex> lock(mutex_);

    try {
        switch (req.type) {
            case AccountReq::Status:
                // 返回账户状态信息
                std::cout << "Account status requested. Available: " << asset_.avail << ", Margin: " << asset_.margin
                          << std::endl;
                break;

            case AccountReq::OrderBook:
                // 返回订单簿信息
                std::cout << "Order book requested. Active orders: " << active_orders_.size() << std::endl;
                break;

            case AccountReq::Order:
                // 返回特定订单信息
                if (req.target_id != 0) {
                    auto it = active_orders_.find(req.target_id);
                    if (it != active_orders_.end()) {
                        std::cout << "Order " << req.target_id << " status: " << static_cast<int>(it->second.status)
                                  << std::endl;
                    }
                }
                break;

            case AccountReq::PositionBook:
                // 返回持仓信息
                std::cout << "Position book requested. Long positions: " << positions_.long_positions.size()
                          << ", Short positions: " << positions_.short_positions.size() << std::endl;
                break;
        }

        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error requesting account info: " << e.what() << std::endl;
        return false;
    }
}

void BrokerSim::process_order_matching() {
    std::lock_guard<std::mutex> lock(mutex_);

    for (auto it = active_orders_.begin(); it != active_orders_.end();) {
        Order& order = it->second;

        // 跳过已完成的订单
        if (order.status == enums::OrderStatus::Filled || order.status == enums::OrderStatus::Cancelled ||
            order.status == enums::OrderStatus::Error) {
            ++it;
            continue;
        }

        // 尝试匹配订单
        match_order_with_market_data(order);

        // 如果订单已完成，从活跃订单中移除
        if (order.status == enums::OrderStatus::Filled || order.status == enums::OrderStatus::Cancelled) {
            it = active_orders_.erase(it);
        } else {
            ++it;
        }
    }
}

void BrokerSim::match_order_with_market_data(Order& order) {
    double market_price = generate_market_price(order);

    // 检查是否可以成交
    if (can_execute_order(order, market_price)) {
        // 计算成交数量（这里简化处理，假设全部成交）
        int64_t volume = order.volume_left;

        // 执行成交
        execute_trade(order, market_price, volume);
    }
}

bool BrokerSim::can_execute_order(const Order& order, double market_price) {
    if (order.price_type == enums::PriceType::Limit) {
        if (order.side == enums::Side::Buy) {
            return market_price <= order.limit_price;
        } else {
            return market_price >= order.limit_price;
        }
    }

    // 市价单直接成交
    return true;
}

void BrokerSim::execute_trade(Order& order, double price, int64_t volume) {
    // 创建成交记录
    Trade trade;
    trade.trade_id = infra::time::now_time(); // 使用时间戳作为成交ID
    trade.order_id = order.order_id;
    trade.instrument_id = order.instrument_id;
    trade.exchange_id = order.exchange_id;
    trade.instrument_type = order.instrument_type;
    trade.side = order.side;
    trade.offset = order.offset;
    trade.hedge_flag = order.hedge_flag;
    trade.price = price;
    trade.volume = volume;
    trade.trade_time = infra::time::now_time();

    // 计算手续费
    if (enable_commission_) {
        trade.commission = calculate_commission(trade);
    } else {
        trade.commission = 0.0;
    }

    trade.tax = 0.0; // 模拟盘暂不考虑税费

    // 添加到成交记录
    trades_[trade.trade_id] = trade;

    // 更新订单状态
    order.volume_left -= volume;
    if (order.volume_left == 0) {
        update_order_status(order.order_id, enums::OrderStatus::Filled);
    } else {
        update_order_status(order.order_id, enums::OrderStatus::PartialFilledActive);
    }

    // 更新持仓和资金
    update_position(trade);
    update_asset(trade);

    std::cout << "Trade executed: Order " << order.order_id << " " << (order.side == enums::Side::Buy ? "BUY" : "SELL")
              << " " << volume << " @ " << price << std::endl;
}

void BrokerSim::add_order(const Order& order) { active_orders_[order.order_id] = order; }

void BrokerSim::remove_order(uint64_t order_id) { active_orders_.erase(order_id); }

void BrokerSim::update_order_status(uint64_t order_id, enums::OrderStatus status) {
    auto it = active_orders_.find(order_id);
    if (it != active_orders_.end()) {
        it->second.status = status;
        it->second.update_time = infra::time::now_time();
    }
}

void BrokerSim::update_position(const Trade& trade) {
    // 创建或更新持仓
    auto direction = enums::side2direction(trade.side);
    auto position_ptr = positions_.get(trade.instrument_id, trade.exchange_id, direction);

    if (position_ptr != nullptr) {
        // 更新现有持仓
        position_ptr->update_time = infra::time::now_time();
        position_ptr->position_cost_price =
            (position_ptr->volume * position_ptr->position_cost_price + trade.volume * trade.price) /
            (position_ptr->volume + trade.volume);
        position_ptr->volume += trade.volume;
    } else {
        // 创建新持仓
        Position position;
        position.update_time = infra::time::now_time();
        position.trading_day = trade.trading_day;
        position.position_cost_price = trade.price;
        position.volume = trade.volume;
        position.exchange_id = trade.exchange_id;
        position.instrument_id = trade.instrument_id;
        position.instrument_type = trade.instrument_type;
        position.direction = direction;
        position.unrealized_pnl = 0.0;

        positions_.set(position);
    }
}

void BrokerSim::calculate_unrealized_pnl() {
    // 这里可以添加根据最新市场价格计算未实现盈亏的逻辑
    // 暂时简化处理
}

void BrokerSim::update_asset(const Trade& trade) {
    double trade_value = trade.price * trade.volume;

    if (trade.side == enums::Side::Buy) {
        // 买入：减少可用资金
        asset_.avail -= trade_value;
    } else {
        // 卖出：增加可用资金
        asset_.avail += trade_value;
    }

    // 扣除手续费
    if (enable_commission_) {
        asset_.avail -= trade.commission;
        asset_.accumulated_fee += trade.commission;
    }

    asset_.update_time = infra::time::now_time();
}

double BrokerSim::calculate_commission(const Trade& trade) {
    double trade_value = trade.price * trade.volume;
    return trade_value * commission_rate_;
}

double BrokerSim::generate_market_price(const Order& order) {
    // 简单的模拟市场价格生成
    // 在实际应用中，这里应该基于真实的市场数据或更复杂的模拟逻辑

    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::normal_distribution<> d(0.0, 0.001); // 0.1%的标准差

    double base_price = order.limit_price;
    if (base_price == 0.0) {
        base_price = 100.0; // 默认价格
    }

    double price_change = d(gen);
    double market_price = base_price * (1.0 + price_change);

    // 应用滑点
    if (enable_slippage_) {
        if (order.side == enums::Side::Buy) {
            market_price *= (1.0 + slippage_rate_); // 买入时价格略高
        } else {
            market_price *= (1.0 - slippage_rate_); // 卖出时价格略低
        }
    }

    return market_price;
}

} // namespace btra::broker