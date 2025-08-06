#pragma once

#include <qdatetime.h>
#include <qlist.h>

#include <QList>
#include <QString>
#include <QVector>
#include <cstddef>
#include <limits>
#include <utility>

#include "guidb/resource.h"
#include "infra/common.h"

namespace btra::gui {

/**
 * @brief 订单簿档位数据结构
 *
 * 包含订单簿中单个价格档位的信息，用于表示买卖盘中的每个价格层级。
 * 该结构体定义了订单簿中每个档位的基本属性，包括价格、挂单量、订单数量等。
 *
 * 典型用法：
 * @code
 * OrderBookLevel bid(50000.0, 1000, 5, "bid");  // 创建买单档位
 * OrderBookLevel ask(50001.0, 800, 3, "ask");   // 创建卖单档位
 * @endcode
 */
struct OrderBookLevel {
    double price;   ///< 价格档位，表示该档位的价格水平（如：50000.0）
    qint64 volume;  ///< 挂单量，表示该价格档位的总挂单数量（如：1000）
    int orderCount; ///< 订单数量，表示该价格档位的订单笔数（如：5）
    QString side;   ///< 买卖方向，标识该档位是买单("bid")还是卖单("ask")

    /**
     * @brief 默认构造函数
     *
     * 初始化所有成员变量为0，创建一个空的订单簿档位。
     * 适用于需要后续填充数据的场景。
     */
    OrderBookLevel() : price(0), volume(0), orderCount(0) {}

    /**
     * @brief 带参数构造函数
     * @param p 价格档位，该档位的价格水平（必须大于0）
     * @param vol 挂单量，该价格档位的总挂单数量（必须大于等于0）
     * @param count 订单数量，该价格档位的订单笔数（必须大于等于0）
     * @param s 买卖方向，必须是"bid"(买单)或"ask"(卖单)
     *
     * 创建一个完整的订单簿档位，所有参数都会被验证。
     */
    OrderBookLevel(double p, qint64 vol, int count, const QString &s)
        : price(p), volume(vol), orderCount(count), side(s) {}
};

class OrderBook {
public:
    OrderBook() : m_bestBid(0), m_bestAsk(0), timeframe_(QDateTime::currentDateTime()) {}
    
    ACCESSFUNCS(double, best_bid, m_bestBid)
    ACCESSFUNCS(double, best_ask, m_bestAsk)

    const QVector<OrderBookLevel> &get_bids() const { return m_bids; }
    const QVector<OrderBookLevel> &get_asks() const { return m_asks; }
    QDateTime get_timeframe() const { return timeframe_; }

    void update(QVector<OrderBookLevel> &&bits, QVector<OrderBookLevel> &&asks, QDateTime timeframe) {
        m_bids = std::forward<QVector<OrderBookLevel>>(bits);
        m_asks = std::forward<QVector<OrderBookLevel>>(asks);
        timeframe_ = timeframe;

        m_bestBid = 0;
        for (auto &level : m_bids) {
            m_bestBid = std::max(m_bestBid, level.price);
        }
        m_bestAsk = std::numeric_limits<double>::max();
        for (auto &level : m_asks) {
            m_bestAsk = std::min(m_bestAsk, level.price);
        }
    }

private:
    QVector<OrderBookLevel> m_bids; ///< 买单数据容器，存储所有买单档位，按价格降序排列
    QVector<OrderBookLevel> m_asks; ///< 卖单数据容器，存储所有卖单档位，按价格升序排列
    double m_bestBid;               ///< 当前最佳买价（最高买价），用于快速访问
    double m_bestAsk;               ///< 当前最佳卖价（最低卖价），用于快速访问
    QDateTime timeframe_;
};

class OrderBookSet : public Resource {
public:
    size_t size() const { return m_books_.size(); }
    bool empty() const { return m_books_.isEmpty(); }
    OrderBook &last_orderbook() { return m_books_.last(); }
    void add_orderbook(OrderBook &&input) { m_books_.append(std::forward<OrderBook>(input)); }

    void pop_front() { 
        if (!m_books_.isEmpty()) {
            m_books_.pop_front(); 
        }
    }

private:
    QList<OrderBook> m_books_;
};
DECLARE_SPTR(OrderBookSet);

} // namespace btra::gui