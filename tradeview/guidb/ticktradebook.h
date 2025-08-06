#pragma once

#include <QDate>
#include <QList>
#include <QString>
#include <cstddef>

#include "guidb/resource.h"
#include "infra/common.h"

namespace btra::gui {

/**
 * @brief 逐笔成交数据结构
 *
 * 包含单笔成交的所有信息，用于存储和传递逐笔成交数据
 */
struct TickTradeData {
    QDateTime timestamp; ///< 成交时间戳
    double price;        ///< 成交价格
    qint64 volume;       ///< 成交数量
    QString direction;   ///< 成交方向 ("buy" 或 "sell")
    QString tradeId;     ///< 成交ID
    QString symbol;      ///< 交易对符号

    /**
     * @brief 默认构造函数
     * 初始化价格和成交量为0
     */
    TickTradeData() : price(0), volume(0) {}

    /**
     * @brief 带参数构造函数
     * @param ts 时间戳
     * @param p 成交价格
     * @param vol 成交数量
     * @param dir 成交方向
     * @param id 成交ID
     * @param sym 交易对符号
     */
    TickTradeData(const QDateTime& ts, double p, qint64 vol, const QString& dir, const QString& id, const QString& sym)
        : timestamp(ts), price(p), volume(vol), direction(dir), tradeId(id), symbol(sym) {}
};

class TickTradeBook : public Resource {
public:
    int count() const { return m_tickTrades.count(); }
    size_t size() const { return m_tickTrades.size(); }
    bool empty() const { return m_tickTrades.isEmpty(); }
    void pop_front() { m_tickTrades.pop_front(); }

    ACCESSFUNCS(int, max_count, m_maxCount)
    ACCESSFUNCS(double, latest_price, m_latestPrice)
    ACCESSFUNCS(qint64, total_volume, m_totalVolume)

    void add(const TickTradeData& data) {
        m_tickTrades.append(data);
        if (m_tickTrades.count() > m_maxCount) {
            m_tickTrades.pop_back();
        }
        m_latestPrice = data.price;

        m_totalVolume = 0;
        for (const auto& tick : m_tickTrades) {
            m_totalVolume += tick.volume;
        }
    }
    const TickTradeData& operator[](size_t i) const { return m_tickTrades[i]; }

    const TickTradeData& at(size_t i) const { return m_tickTrades[i]; }

private:
    QList<TickTradeData> m_tickTrades;
    int m_maxCount{1000};
    double m_latestPrice{0};
    qint64 m_totalVolume{0};
};
DECLARE_SPTR(TickTradeBook);

} // namespace btra::gui