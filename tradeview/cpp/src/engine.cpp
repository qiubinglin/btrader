#include "engine.h"
#include <algorithm>
#include <random>
#include <thread>
#include <chrono>
#include <iostream>

TradingEngine::TradingEngine() 
    : next_order_id_(1), next_trade_id_(1), simulation_running_(false) {
    // 初始化默认股票
    current_prices_["AAPL"] = 150.0;
    current_prices_["GOOGL"] = 2800.0;
    current_prices_["TSLA"] = 800.0;
    
    for (const auto& [symbol, price] : current_prices_) {
        updateOrderBook(symbol);
    }
}

TradingEngine::~TradingEngine() {
    stopMarketDataSimulation();
}

uint64_t TradingEngine::addOrder(const Order& order) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    Order new_order = order;
    new_order.id = next_order_id_++;
    new_order.filled_quantity = 0;
    new_order.status = "pending";
    new_order.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    orders_[new_order.id] = new_order;
    
    processOrder(orders_[new_order.id]);
    
    if (order_callback_) {
        order_callback_(orders_[new_order.id]);
    }
    
    return new_order.id;
}

bool TradingEngine::cancelOrder(uint64_t orderId) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = orders_.find(orderId);
    if (it != orders_.end() && it->second.status == "pending") {
        it->second.status = "cancelled";
        if (order_callback_) {
            order_callback_(it->second);
        }
        return true;
    }
    return false;
}

Order TradingEngine::getOrder(uint64_t orderId) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = orders_.find(orderId);
    return (it != orders_.end()) ? it->second : Order{};
}

std::vector<Order> TradingEngine::getOrders() {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<Order> result;
    for (const auto& [id, order] : orders_) {
        result.push_back(order);
    }
    return result;
}

OrderBook TradingEngine::getOrderBook(const std::string& symbol) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = order_books_.find(symbol);
    return (it != order_books_.end()) ? it->second : OrderBook{};
}

std::vector<Trade> TradingEngine::getTrades(const std::string& symbol, int limit) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = trades_.find(symbol);
    if (it != trades_.end()) {
        std::vector<Trade> result = it->second;
        if (result.size() > limit) {
            result.resize(limit);
        }
        return result;
    }
    return {};
}

double TradingEngine::getCurrentPrice(const std::string& symbol) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = current_prices_.find(symbol);
    return (it != current_prices_.end()) ? it->second : 0.0;
}

void TradingEngine::setTradeCallback(std::function<void(const Trade&)> callback) {
    trade_callback_ = callback;
}

void TradingEngine::setOrderUpdateCallback(std::function<void(const Order&)> callback) {
    order_callback_ = callback;
}

void TradingEngine::processOrder(Order& order) {
    if (order.type == "market") {
        // 市价单立即成交
        order.filled_quantity = order.quantity;
        order.status = "filled";
        order.price = current_prices_[order.symbol];
        
        // 创建成交记录
        Trade trade;
        trade.id = next_trade_id_++;
        trade.symbol = order.symbol;
        trade.price = order.price;
        trade.quantity = order.quantity;
        trade.side = order.side;
        trade.timestamp = order.timestamp;
        
        trades_[order.symbol].insert(trades_[order.symbol].begin(), trade);
        if (trades_[order.symbol].size() > 1000) {
            trades_[order.symbol].pop_back();
        }
        
        if (trade_callback_) {
            trade_callback_(trade);
        }
        
        // 更新价格
        double price_impact = (order.side == "buy" ? 0.01 : -0.01) * (order.quantity / 1000.0);
        current_prices_[order.symbol] += price_impact;
        updateOrderBook(order.symbol);
    }
}

void TradingEngine::updateOrderBook(const std::string& symbol) {
    OrderBook& book = order_books_[symbol];
    book.symbol = symbol;
    book.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    double current_price = current_prices_[symbol];
    book.bids.clear();
    book.asks.clear();
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> size_dist(100, 1000);
    std::uniform_int_distribution<> orders_dist(1, 10);
    
    // 生成买盘
    for (int i = 0; i < 10; ++i) {
        OrderBookLevel level;
        level.price = current_price - 0.01 * (i + 1);
        level.quantity = size_dist(gen);
        level.orders = orders_dist(gen);
        book.bids.push_back(level);
    }
    
    // 生成卖盘
    for (int i = 0; i < 10; ++i) {
        OrderBookLevel level;
        level.price = current_price + 0.01 * (i + 1);
        level.quantity = size_dist(gen);
        level.orders = orders_dist(gen);
        book.asks.push_back(level);
    }
}

void TradingEngine::startMarketDataSimulation() {
    if (simulation_running_) return;
    
    simulation_running_ = true;
    std::thread([this]() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> price_change(-0.05, 0.05);
        std::uniform_int_distribution<> interval(1000, 3000);
        
        while (simulation_running_) {
            std::this_thread::sleep_for(std::chrono::milliseconds(interval(gen)));
            
            if (!simulation_running_) break;
            
            std::lock_guard<std::mutex> lock(mutex_);
            for (auto& [symbol, price] : current_prices_) {
                price += price_change(gen);
                if (price < 1.0) price = 1.0;
                updateOrderBook(symbol);
            }
        }
    }).detach();
}

void TradingEngine::stopMarketDataSimulation() {
    simulation_running_ = false;
}
