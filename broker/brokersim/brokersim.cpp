#include "brokersim.h"

#include <algorithm>
#include <chrono>
#include <cstring>
#include <iostream>
#include <random>
#include <vector>

#include "constants.h"
#include "core/types.h"
#include "extension/globalparams.h"
#include "infra/singleton.h"
#include "infra/time.h"

// 常量定义
namespace {
constexpr double DEFAULT_INITIAL_CAPITAL = 1000000.0; // 默认初始资金100万
constexpr double DEFAULT_COMMISSION_RATE = 0.0003;    // 默认手续费率0.03%
constexpr double DEFAULT_SLIPPAGE_RATE = 0.0001;      // 默认滑点率0.01%
constexpr int DEFAULT_MATCHING_INTERVAL_MS = 100;     // 默认撮合间隔100ms
constexpr size_t MAX_DEPTH_LEVELS = 20;               // 最大深度级别
constexpr double MIN_VALID_PRICE = 0.000001;          // 最小有效价格
constexpr double MIN_VALID_VOLUME = 0.000001;         // 最小有效数量
} // namespace

namespace btra::broker {

BrokerSim::BrokerSim() {
    // 初始化资产
    asset_.update_time = infra::time::now_time();
    asset_.avail = DEFAULT_INITIAL_CAPITAL;
    asset_.margin = 0.0;
    asset_.accumulated_fee = 0.0;

    // 初始化持仓
    positions_ = PositionBook{};
}

void BrokerSim::setup(const Json::json& cfg) {
    try {
        Json::json simulation_cfg = cfg["extra"];

        // 设置手续费率
        commission_rate_ = simulation_cfg.value("commission_rate", DEFAULT_COMMISSION_RATE);

        // 设置滑点率
        slippage_rate_ = simulation_cfg.value("slippage_rate", DEFAULT_SLIPPAGE_RATE);

        // 设置功能开关
        enable_slippage_ = simulation_cfg.value("enable_slippage", true);
        enable_commission_ = simulation_cfg.value("enable_commission", true);

        // 设置初始资金
        if (simulation_cfg.contains("initial_capital")) {
            asset_.avail = simulation_cfg["initial_capital"].get<double>();
        }

        std::cout << "BrokerSim setup completed. Commission rate: " << commission_rate_
                  << ", Slippage rate: " << slippage_rate_ << std::endl;

        depth_callboard_.init(INSTANCE(GlobalParams).root_dir, PAGE_SIZE, sizeof(InstrumentDepth<10>), false);

    } catch (const std::exception& e) {
        std::cerr << "Error in BrokerSim setup: " << e.what() << std::endl;
        throw;
    }
}

void BrokerSim::start() {
    std::lock_guard<std::mutex> lock(mutex_);

    if (running_) {
        return;
    }

    running_ = true;
    state_ = enums::BrokerState::Ready;

    // 启动订单匹配线程
    matching_thread_ = std::thread([this]() {
        while (running_) {
            process_order_matching();
            std::this_thread::sleep_for(std::chrono::milliseconds(DEFAULT_MATCHING_INTERVAL_MS));
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
        order.insert_time = input.insert_time;
        order.update_time = input.insert_time;

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

bool BrokerSim::cancel_order(const OrderCancel& input) {
    std::lock_guard<std::mutex> lock(mutex_);

    try {
        auto it = active_orders_.find(input.target_order_id);
        if (it == active_orders_.end()) {
            std::cerr << "Order " << input.target_order_id << " not found for cancellation" << std::endl;
            OrderActionResp resp;
            resp.order_action_id = input.order_id;
            resp.order_id = input.target_order_id;
            resp.resp_type = enums::BrokerRespType::OrderCancel;
            resp.error_id = -2;
            notify_response(resp);
            return false;
        }

        Order& order = it->second;
        // Check if order can be cancelled
        // Orders can only be cancelled in certain states:
        // - Submitted: Order submitted but not yet processed
        // - Pending: Order waiting for market conditions
        // - PartialFilledActive: Partially filled order still active
        //
        // Orders cannot be cancelled in these states:
        // - Filled: Already completely executed
        // - Cancelled: Already cancelled
        // - Error: Order in error state
        // - Lost: Order lost due to system issues
        // - PartialFilledNotActive: Partially filled but no longer active
        if (order.status != enums::OrderStatus::Submitted && order.status != enums::OrderStatus::Pending &&
            order.status != enums::OrderStatus::PartialFilledActive) {
            std::cerr << "Order " << input.target_order_id << " cannot be cancelled in status "
                      << static_cast<int>(order.status) << " (" << get_order_status_string(order.status) << ")"
                      << std::endl;
            OrderActionResp resp;
            resp.order_action_id = input.order_id;
            resp.order_id = input.target_order_id;
            resp.resp_type = enums::BrokerRespType::OrderCancel;
            resp.error_id = -1;
            notify_response(resp);
            return false;
        }

        // 更新订单状态
        update_order_status(input.target_order_id, enums::OrderStatus::Cancelled);
        OrderActionResp resp;
        resp.order_action_id = input.order_id;
        resp.order_id = input.target_order_id;
        resp.resp_type = enums::BrokerRespType::OrderCancel;
        resp.error_id = 0;
        notify_response(resp);

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
    // 获取实时深度数据
    InstrumentDepth<20> depth;
    depth_callboard_.get(order.instrument_id, depth);

    // 如果没有深度数据，执行模拟成交
    if (depth.real_depth_size == 0) {
        handle_no_depth_execution(order);
        return;
    }

    // 根据订单方向选择撮合价格和数量
    std::vector<std::pair<double, VolumeType>> price_levels = build_price_levels(depth, order.side);

    // 如果没有可撮合的价格
    if (price_levels.empty()) {
        if (order.status == enums::OrderStatus::Submitted) {
            order.status = enums::OrderStatus::Pending;
            order.update_time = infra::time::now_time();
        }
        return;
    }

    // 尝试逐级撮合
    VolumeType remaining_volume = order.volume_left;
    double weighted_avg_price = 0.0;
    VolumeType total_traded_volume = 0.0;
    std::vector<std::pair<double, VolumeType>> executed_trades; // 记录所有成交的价格和数量

    for (const auto& [price, volume] : price_levels) {
        if (remaining_volume <= 0) break;

        // 应用滑点计算实际成交价格
        double final_price = apply_slippage(price, order.side);

        // 使用加了滑点后的价格检查是否可以成交
        if (can_execute_order(order, final_price)) {
            // 计算当前价格级别的成交数量
            VolumeType trade_volume = std::min(remaining_volume, volume);

            // 检查资金是否足够（买单需要检查）
            if (!check_funds_sufficiency(order, final_price, trade_volume)) {
                continue; // 跳过这个价格级别，尝试下一个
            }

            // 记录成交信息（不立即执行，先收集所有成交）
            executed_trades.push_back({final_price, trade_volume});

            // 更新加权平均价格
            weighted_avg_price += final_price * trade_volume;
            total_traded_volume += trade_volume;
            remaining_volume -= trade_volume;

            std::cout << "Partial fill: Order " << order.order_id << " "
                      << (order.side == enums::Side::Buy ? "BUY" : "SELL") << " " << static_cast<int64_t>(trade_volume)
                      << " @ " << final_price << " (Depth price: " << price << ")" << std::endl;
        } else {
            // 价格不满足条件，停止撮合
            break;
        }
    }

    // 如果有成交，生成汇总的Trade实例
    if (total_traded_volume > 0) {
        double avg_price = weighted_avg_price / total_traded_volume;

        // 生成汇总的Trade实例
        Trade summary_trade = generate_summary_trade(order, avg_price, total_traded_volume, executed_trades);

        // 执行汇总成交
        execute_summary_trade(order, summary_trade);

        std::cout << "Order " << order.order_id << " matched: " << static_cast<int64_t>(total_traded_volume)
                  << " @ avg price " << avg_price << std::endl;
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
    try {
        // 创建或更新持仓
        auto direction = enums::side2direction(trade.side);
        auto position_ptr = positions_.get(trade.instrument_id, trade.exchange_id, direction);

        if (position_ptr != nullptr) {
            // 更新现有持仓
            update_existing_position(*position_ptr, trade);
        } else {
            // 创建新持仓
            create_new_position(trade, direction);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error updating position: " << e.what() << std::endl;
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

double BrokerSim::apply_slippage(double base_price, enums::Side side) {
    if (!enable_slippage_) {
        return base_price;
    }

    if (side == enums::Side::Buy) {
        // 买入时价格略高（滑点）
        return base_price * (1.0 + slippage_rate_);
    } else {
        // 卖出时价格略低（滑点）
        return base_price * (1.0 - slippage_rate_);
    }
}

std::string BrokerSim::get_order_status_string(enums::OrderStatus status) {
    switch (status) {
        case enums::OrderStatus::Unknown:
            return "Unknown";
        case enums::OrderStatus::Submitted:
            return "Submitted";
        case enums::OrderStatus::Pending:
            return "Pending";
        case enums::OrderStatus::Cancelled:
            return "Cancelled";
        case enums::OrderStatus::Error:
            return "Error";
        case enums::OrderStatus::Filled:
            return "Filled";
        case enums::OrderStatus::PartialFilledNotActive:
            return "PartialFilledNotActive";
        case enums::OrderStatus::PartialFilledActive:
            return "PartialFilledActive";
        case enums::OrderStatus::Lost:
            return "Lost";
        default:
            return "Invalid";
    }
}

double BrokerSim::generate_market_price(const Order& order) {
    // 简单的模拟市场价格生成
    // 在实际应用中，这里应该基于真实的市场数据或更复杂的模拟逻辑

    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::normal_distribution<> d(0.0, 0.001); // 0.1%的标准差

    double base_price = order.limit_price;
    if (base_price <= 0.0) {
        base_price = 100.0; // 默认价格
    }

    double price_change = d(gen);
    double market_price = base_price * (1.0 + price_change);

    // 注意：这里不应用滑点，滑点在市价单处理中单独应用
    return market_price;
}

Trade BrokerSim::generate_summary_trade(const Order& order, double avg_price, VolumeType total_volume,
                                        const std::vector<std::pair<double, VolumeType>>& executed_trades) {
    // 创建汇总成交记录
    Trade summary_trade;

    // 生成唯一的成交ID（使用时间戳+订单ID的组合）
    summary_trade.trade_id = infra::time::now_time() * 1000000 + (order.order_id % 1000000);

    // 基本信息
    summary_trade.order_id = order.order_id;
    summary_trade.instrument_id = order.instrument_id;
    summary_trade.exchange_id = order.exchange_id;
    summary_trade.instrument_type = order.instrument_type;
    summary_trade.side = order.side;
    summary_trade.offset = order.offset;
    summary_trade.hedge_flag = order.hedge_flag;

    // 成交价格和数量（使用加权平均价格）
    summary_trade.price = avg_price;
    summary_trade.volume = total_volume;

    // 时间信息
    summary_trade.trade_time = infra::time::now_time();
    summary_trade.trading_day = order.trading_day;

    // 计算手续费（基于加权平均价格）
    if (enable_commission_) {
        double trade_value = avg_price * total_volume;
        summary_trade.commission = trade_value * commission_rate_;
    } else {
        summary_trade.commission = 0.0;
    }

    summary_trade.tax = 0.0; // 模拟盘暂不考虑税费

    // 设置外部ID（模拟盘使用内部ID）
    std::string external_order_id = "SIM_" + std::to_string(order.order_id);
    std::string external_trade_id = "SIM_" + std::to_string(summary_trade.trade_id);

    // 复制到Array类型
    std::strncpy(summary_trade.external_order_id.value, external_order_id.c_str(),
                 std::min(external_order_id.length(), static_cast<size_t>(EXTERNAL_ID_LEN - 1)));
    std::strncpy(summary_trade.external_trade_id.value, external_trade_id.c_str(),
                 std::min(external_trade_id.length(), static_cast<size_t>(EXTERNAL_ID_LEN - 1)));

    std::cout << "Generated summary trade: ID=" << summary_trade.trade_id << ", Order=" << order.order_id
              << ", Price=" << avg_price << ", Volume=" << static_cast<int64_t>(total_volume)
              << ", Commission=" << summary_trade.commission << std::endl;

    return summary_trade;
}

void BrokerSim::execute_summary_trade(Order& order, const Trade& summary_trade) {
    // 更新订单状态
    order.volume_left -= static_cast<int64_t>(summary_trade.volume);
    if (order.volume_left == 0) {
        update_order_status(order.order_id, enums::OrderStatus::Filled);
    } else {
        update_order_status(order.order_id, enums::OrderStatus::PartialFilledActive);
    }

    // 更新持仓和资金
    update_position(summary_trade);
    update_asset(summary_trade);

    notify_response(summary_trade);

    std::cout << "Summary trade executed: Order " << order.order_id << " "
              << (order.side == enums::Side::Buy ? "BUY" : "SELL") << " " << summary_trade.volume << " @ "
              << summary_trade.price << " (Total Commission: " << summary_trade.commission << ")" << std::endl;
}

// 辅助方法实现
void BrokerSim::handle_no_depth_execution(Order& order) {
    double execution_price;

    if (order.price_type == enums::PriceType::Limit) {
        // 限价单：直接按限价成交，无需滑点
        execution_price = order.limit_price;
        std::cout << "No depth data available, limit order " << order.order_id
                  << " executed at limit price: " << execution_price << " symbol: " << order.instrument_id.to_string()
                  << std::endl;
    } else {
        // 市价单：生成模拟价格并加滑点
        double simulated_price = generate_market_price(order);
        execution_price = apply_slippage(simulated_price, order.side);
        std::cout << "No depth data available, market order " << order.order_id
                  << " executed at simulated price with slippage: " << execution_price << " (base: " << simulated_price
                  << ")" << " symbol: " << order.instrument_id.to_string() << std::endl;
    }

    // 检查资金是否足够（买单需要检查）
    if (order.side == enums::Side::Buy) {
        double required_cash = execution_price * order.volume_left;
        if (asset_.avail < required_cash) {
            std::cout << "Insufficient funds for order " << order.order_id << ". Required: " << required_cash
                      << ", Available: " << asset_.avail << ". Cannot execute simulated trade." << std::endl;

            // 更新订单状态为错误
            update_order_status(order.order_id, enums::OrderStatus::Error);
            return;
        }
    }

    // 创建模拟成交记录
    Trade simulated_trade = generate_summary_trade(order, execution_price, order.volume_left, {});

    // 执行模拟成交
    execute_summary_trade(order, simulated_trade);
}

bool BrokerSim::check_funds_sufficiency(const Order& order, double price, VolumeType volume) {
    if (order.side == enums::Side::Buy) {
        double required_cash = price * volume;
        if (asset_.avail < required_cash) {
            std::cout << "Insufficient funds for order " << order.order_id << ". Required: " << required_cash
                      << ", Available: " << asset_.avail << ". Skipping this price level." << std::endl;
            return false;
        }
    }
    return true;
}

std::vector<std::pair<double, VolumeType>> BrokerSim::build_price_levels(const InstrumentDepth<20>& depth,
                                                                         enums::Side side) {
    std::vector<std::pair<double, VolumeType>> price_levels;
    price_levels.reserve(MAX_DEPTH_LEVELS); // 预分配内存

    if (side == enums::Side::Buy) {
        // 买单：寻找卖盘价格进行撮合
        for (size_t i = 0; i < depth.real_depth_size && i < MAX_DEPTH_LEVELS; ++i) {
            if (depth.ask_price[i] > MIN_VALID_PRICE && depth.ask_volume[i] > MIN_VALID_VOLUME) {
                price_levels.push_back({depth.ask_price[i], depth.ask_volume[i]});
            }
        }
    } else {
        // 卖单：寻找买盘价格进行撮合
        for (size_t i = 0; i < depth.real_depth_size && i < MAX_DEPTH_LEVELS; ++i) {
            if (depth.bid_price[i] > MIN_VALID_PRICE && depth.bid_volume[i] > MIN_VALID_VOLUME) {
                price_levels.push_back({depth.bid_price[i], depth.bid_volume[i]});
            }
        }
    }

    return price_levels;
}

// 持仓管理辅助方法实现
void BrokerSim::update_existing_position(Position& position, const Trade& trade) {
    position.update_time = infra::time::now_time();

    // 计算新的平均成本价格
    double total_cost = position.volume * position.position_cost_price;
    double new_cost = trade.volume * trade.price;
    double total_volume = position.volume + trade.volume;

    if (total_volume > 0) {
        position.position_cost_price = (total_cost + new_cost) / total_volume;
    }

    position.volume += trade.volume;
}

void BrokerSim::create_new_position(const Trade& trade, enums::Direction direction) {
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

} // namespace btra::broker