#ifndef TRADING_ENGINE_H
#define TRADING_ENGINE_H

#include <vector>
#include <string>
#include <map>
#include <functional>
#include <mutex>
#include <memory>

struct Order {
    uint64_t id;
    std::string symbol;
    std::string side;  // "buy" or "sell"
    std::string type;  // "market" or "limit"
    double price;
    int quantity;
    int filled_quantity;
    std::string status; // "pending", "filled", "cancelled"
    long timestamp;
};

struct Trade {
    uint64_t id;
    std::string symbol;
    double price;
    int quantity;
    std::string side;
    long timestamp;
};

struct OrderBookLevel {
    double price;
    int quantity;
    int orders;
};

struct OrderBook {
    std::vector<OrderBookLevel> bids;
    std::vector<OrderBookLevel> asks;
    std::string symbol;
    long timestamp;
};

class TradingEngine {
public:
    TradingEngine();
    ~TradingEngine();
    
    // 订单操作
    uint64_t addOrder(const Order& order);
    bool cancelOrder(uint64_t orderId);
    Order getOrder(uint64_t orderId);
    std::vector<Order> getOrders();
    
    // 市场数据
    OrderBook getOrderBook(const std::string& symbol);
    std::vector<Trade> getTrades(const std::string& symbol, int limit = 100);
    double getCurrentPrice(const std::string& symbol);
    
    // 回调设置
    void setTradeCallback(std::function<void(const Trade&)> callback);
    void setOrderUpdateCallback(std::function<void(const Order&)> callback);
    
    // 市场数据模拟
    void startMarketDataSimulation();
    void stopMarketDataSimulation();
    
private:
    std::map<uint64_t, Order> orders_;
    std::map<std::string, std::vector<Trade>> trades_;
    std::map<std::string, OrderBook> order_books_;
    std::map<std::string, double> current_prices_;
    
    std::function<void(const Trade&)> trade_callback_;
    std::function<void(const Order&)> order_callback_;
    
    uint64_t next_order_id_;
    uint64_t next_trade_id_;
    std::mutex mutex_;
    bool simulation_running_;
    
    void processOrder(Order& order);
    void matchOrders(const std::string& symbol);
    void generateMarketData();
    void updateOrderBook(const std::string& symbol);
};

#endif // TRADING_ENGINE_H