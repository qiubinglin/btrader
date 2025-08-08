#pragma once

#include <qdatetime.h>

#include <QDateTime>
#include <QTime>

#include "guitime.h"

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
     * @brief 带参数构造函数（根据传入的时间戳计算包含该时间戳的那一分钟的起始和结束时间）
     * @param ts 时间戳（用于计算包含该时间戳的那一分钟）
     * @param o 开盘价
     * @param h 最高价
     * @param l 最低价
     * @param c 收盘价
     * @param vol 成交量
     * @param amt 成交额
     */
    KLine(const QDateTime& ts, double o, double h, double l, double c, qint64 vol, double amt)
        : open(o), high(h), low(l), close(c), volume(vol), amount(amt) {
        // 计算包含该时间戳的那一分钟的起始时间
        // 将秒和毫秒清零，只保留到分钟级别
        start_time = QDateTime(ts.date(), QTime(ts.time().hour(), ts.time().minute(), 0, 0));

        // 计算结束时间（起始时间加1分钟）
        end_time = start_time.addSecs(60);
    }

    bool contains(const QDateTime& ts) const { return start_time < ts and end_time >= ts; }
};

} // namespace btra::gui