#pragma once

#include <QDateTime>
#include <QTimeZone>

namespace btra::gui {

/**
 * @brief K线数据结构
 *
 * 包含单根K线的所有数据信息，用于存储和传递K线数据
 */
struct KLine {
    QDateTime start_time; ///< 开始时间（含）
    QDateTime end_time;   ///< 结束时间（不含）
    double open{0.0};     ///< 开盘价
    double high{0.0};     ///< 最高价
    double low{0.0};      ///< 最低价
    double close{0.0};    ///< 收盘价
    qint64 volume{0};     ///< 成交量
    double amount{0.0};   ///< 成交额

    /**
     * @brief 默认构造函数
     */
    KLine() = default;

    /**
     * @brief 带参数构造函数（使用开始和结束时间）
     */
    KLine(const QDateTime& start, const QDateTime& end, double o, double h, double l, double c, qint64 vol, double amt)
        : start_time(start), end_time(end), open(o), high(h), low(l), close(c), volume(vol), amount(amt) {}

    /**
     * @brief 根据传入的时间戳，取其所在分钟区间 [start, end) 构造
     */
    KLine(const QDateTime& ts, double o, double h, double l, double c, qint64 vol, double amt)
        : open(o), high(h), low(l), close(c), volume(vol), amount(amt) {
        const qint64 ms = ts.toMSecsSinceEpoch();
        const qint64 startMs = (ms / 60000) * 60000;  // 向下取整到分钟
        start_time = QDateTime::fromMSecsSinceEpoch(startMs, ts.timeZone());
        end_time = start_time.addSecs(60);
    }

    /**
     * @brief 判断时间戳是否落在本K线时间区间内（半开区间 [start, end)）
     */
    inline bool contains(const QDateTime& ts) const { return ts >= start_time && ts < end_time; }
};

} // namespace btra::gui