#pragma once

#include <memory>
#include <mutex>
#include <thread>
#include <unordered_map>

#include "broker/trade_service.h"

namespace btra::broker {

class BrokerSim : public TradeService {
public:
    BrokerSim();
    ~BrokerSim() override = default;

    void setup(const Json::json& cfg) override;
    void start() override;
    void stop() override;
    enums::AccountType get_account_type() const override;
    bool insert_order(const OrderInput& input) override;
    bool cancel_order(const OrderAction& input) override;
    bool req_account_info(const AccountReq& req) override;

private:
    // 模拟成交逻辑
    void process_order_matching();
    void match_order_with_market_data(Order& order);
    bool can_execute_order(const Order& order, double market_price);
    void execute_trade(Order& order, double price, int64_t volume);

    // 订单管理
    void add_order(const Order& order);
    void remove_order(uint64_t order_id);
    void update_order_status(uint64_t order_id, enums::OrderStatus status);

    // 持仓管理
    void update_position(const Trade& trade);
    void calculate_unrealized_pnl();

    // 资金管理
    void update_asset(const Trade& trade);
    double calculate_commission(const Trade& trade);

    // 生成模拟市场数据
    double generate_market_price(const Order& order);

    // 内部状态
    std::map<uint64_t, Order> active_orders_;
    std::map<uint64_t, Trade> trades_;
    Asset asset_;
    PositionBook positions_;

    // 配置参数
    double commission_rate_{0.0003}; // 默认手续费率0.03%
    double slippage_rate_{0.0001};   // 默认滑点率0.01%
    bool enable_slippage_{true};     // 是否启用滑点
    bool enable_commission_{true};   // 是否启用手续费

    // 线程安全
    mutable std::mutex mutex_;

    // 运行状态
    bool running_{false};
    std::thread matching_thread_;
};

} // namespace btra::broker
