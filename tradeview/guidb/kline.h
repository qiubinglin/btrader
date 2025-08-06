#pragma once

#include <QDateTime>

namespace btra::gui {

/**
 * @brief K线数据结构
 *
 * 包含单根K线的所有数据信息，用于存储和传递K线数据
 */
struct KLine {
    QDateTime start_time; ///< 开始时间
    QDateTime end_time;   ///< 结束时间
    double open;          ///< 开盘价
    double high;          ///< 最高价
    double low;           ///< 最低价
    double close;         ///< 收盘价
    qint64 volume;        ///< 成交量
    double amount;        ///< 成交额

    /**
     * @brief 默认构造函数
     * 初始化所有成员变量为0
     */
    KLine() : open(0), high(0), low(0), close(0), volume(0), amount(0) {}

    /**
     * @brief 带参数构造函数（使用开始和结束时间）
     * @param start 开始时间
     * @param end 结束时间
     * @param o 开盘价
     * @param h 最高价
     * @param l 最低价
     * @param c 收盘价
     * @param vol 成交量
     * @param amt 成交额
     */
    KLine(const QDateTime& start, const QDateTime& end, double o, double h, double l, double c, qint64 vol, double amt)
        : start_time(start), end_time(end), open(o), high(h), low(l), close(c), volume(vol), amount(amt) {}

    /**
     * @brief 带参数构造函数（向后兼容，使用单个时间戳作为结束时间）
     * @param ts 时间戳（作为结束时间）
     * @param o 开盘价
     * @param h 最高价
     * @param l 最低价
     * @param c 收盘价
     * @param vol 成交量
     * @param amt 成交额
     */
    KLine(const QDateTime& ts, double o, double h, double l, double c, qint64 vol, double amt)
        : end_time(ts), open(o), high(h), low(l), close(c), volume(vol), amount(amt) {
        // 默认开始时间为结束时间减去1分钟
        start_time = end_time.addSecs(-60);
    }
};

} // namespace btra::gui