#pragma once

#include <QString>
#include <deque>

#include "guidb/kline.h"
#include "guidb/resource.h"
#include "infra/common.h"

namespace btra::gui {

class KLineSet : public Resource {
public:
    int count() const;
    size_t size() const { return klines.size(); }
    void clear();

    /**
     * @brief 设置时间周期
     * @param timeframe 时间周期字符串
     */
    bool set_timeframe(const QString& timeframe);

    /**
     * @brief 获取时间周期
     * @return 时间周期字符串
     */
    const QString& get_timeframe() const;

    double get_max_price() const;
    double get_min_price() const;
    qint64 get_max_volume() const;

    const KLine& at(size_t i) { return klines.at(i); }
    void push_back(const KLine& elm) { klines.push_back(elm); }
    void pop_front() { klines.pop_front(); }
    KLine& back() { return klines.back(); }

private:
    std::deque<KLine> klines;
    QString m_timeframe{"1m"}; ///< 时间周期
    double m_max_price{0};     ///< 最高价格
    double m_min_price{0};     ///< 最低价格
    qint64 m_max_volume{0};    ///< 最大成交量
};
DECLARE_SPTR(KLineSet);

} // namespace btra::gui