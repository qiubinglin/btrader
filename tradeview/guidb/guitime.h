#pragma once

#include <QDateTime>
#include <cstdint>

namespace btra::gui {

/**
 * @brief 时间戳转换工具类
 * 
 * 提供 int64_t timestamp 和 QDateTime 之间的互相转换功能
 * 支持毫秒级时间戳和微秒级时间戳
 */
class GuiTime {
public:
    /**
     * @brief 将 int64_t 毫秒时间戳转换为 QDateTime
     * @param timestamp 毫秒级时间戳（从1970年1月1日开始的毫秒数）
     * @return 对应的 QDateTime 对象
     */
    static QDateTime fromTimestampMs(int64_t timestamp) {
        return QDateTime::fromMSecsSinceEpoch(timestamp);
    }

    /**
     * @brief 将 int64_t 微秒时间戳转换为 QDateTime
     * @param timestamp 微秒级时间戳（从1970年1月1日开始的微秒数）
     * @return 对应的 QDateTime 对象
     */
    static QDateTime fromTimestampUs(int64_t timestamp) {
        return QDateTime::fromMSecsSinceEpoch(timestamp / 1000);
    }

    /**
     * @brief 将 int64_t 纳秒时间戳转换为 QDateTime
     * @param timestamp 纳秒级时间戳（从1970年1月1日开始的纳秒数）
     * @return 对应的 QDateTime 对象
     */
    static QDateTime fromTimestampNs(int64_t timestamp) {
        return QDateTime::fromMSecsSinceEpoch(timestamp / 1000000);
    }

    /**
     * @brief 将 QDateTime 转换为 int64_t 毫秒时间戳
     * @param dateTime QDateTime 对象
     * @return 毫秒级时间戳
     */
    static int64_t toTimestampMs(const QDateTime& dateTime) {
        return dateTime.toMSecsSinceEpoch();
    }

    /**
     * @brief 将 QDateTime 转换为 int64_t 微秒时间戳
     * @param dateTime QDateTime 对象
     * @return 微秒级时间戳
     */
    static int64_t toTimestampUs(const QDateTime& dateTime) {
        return dateTime.toMSecsSinceEpoch() * 1000;
    }

    /**
     * @brief 将 QDateTime 转换为 int64_t 纳秒时间戳
     * @param dateTime QDateTime 对象
     * @return 纳秒级时间戳
     */
    static int64_t toTimestampNs(const QDateTime& dateTime) {
        return dateTime.toMSecsSinceEpoch() * 1000000;
    }

    /**
     * @brief 获取当前时间的毫秒时间戳
     * @return 当前时间的毫秒级时间戳
     */
    static int64_t currentTimestampMs() {
        return QDateTime::currentMSecsSinceEpoch();
    }

    /**
     * @brief 获取当前时间的微秒时间戳
     * @return 当前时间的微秒级时间戳
     */
    static int64_t currentTimestampUs() {
        return QDateTime::currentMSecsSinceEpoch() * 1000;
    }

    /**
     * @brief 获取当前时间的纳秒时间戳
     * @return 当前时间的纳秒级时间戳
     */
    static int64_t currentTimestampNs() {
        return QDateTime::currentMSecsSinceEpoch() * 1000000;
    }

    /**
     * @brief 格式化时间戳为字符串
     * @param timestamp 毫秒级时间戳
     * @param format 时间格式字符串，默认为 "yyyy-MM-dd hh:mm:ss.zzz"
     * @return 格式化后的时间字符串
     */
    static QString formatTimestamp(int64_t timestamp, const QString& format = "yyyy-MM-dd hh:mm:ss.zzz") {
        QDateTime dateTime = fromTimestampMs(timestamp);
        return dateTime.toString(format);
    }

    /**
     * @brief 格式化 QDateTime 为字符串
     * @param dateTime QDateTime 对象
     * @param format 时间格式字符串，默认为 "yyyy-MM-dd hh:mm:ss.zzz"
     * @return 格式化后的时间字符串
     */
    static QString formatDateTime(const QDateTime& dateTime, const QString& format = "yyyy-MM-dd hh:mm:ss.zzz") {
        return dateTime.toString(format);
    }

    /**
     * @brief 检查时间戳是否有效
     * @param timestamp 毫秒级时间戳
     * @return 如果时间戳有效返回 true，否则返回 false
     */
    static bool isValidTimestamp(int64_t timestamp) {
        QDateTime dateTime = fromTimestampMs(timestamp);
        return dateTime.isValid();
    }

    /**
     * @brief 获取时间戳的日期部分
     * @param timestamp 毫秒级时间戳
     * @return 日期字符串，格式为 "yyyy-MM-dd"
     */
    static QString getDateString(int64_t timestamp) {
        QDateTime dateTime = fromTimestampMs(timestamp);
        return dateTime.toString("yyyy-MM-dd");
    }

    /**
     * @brief 获取时间戳的时间部分
     * @param timestamp 毫秒级时间戳
     * @return 时间字符串，格式为 "hh:mm:ss.zzz"
     */
    static QString getTimeString(int64_t timestamp) {
        QDateTime dateTime = fromTimestampMs(timestamp);
        return dateTime.toString("hh:mm:ss.zzz");
    }
};

/*
 * 使用示例：
 * 
 * // 毫秒时间戳转换
 * int64_t timestampMs = 1640995200000; // 2022-01-01 00:00:00
 * QDateTime dateTime = GuiTime::fromTimestampMs(timestampMs);
 * int64_t convertedMs = GuiTime::toTimestampMs(dateTime);
 * 
 * // 微秒时间戳转换
 * int64_t timestampUs = 1640995200000000; // 2022-01-01 00:00:00 (微秒)
 * QDateTime dateTime2 = GuiTime::fromTimestampUs(timestampUs);
 * int64_t convertedUs = GuiTime::toTimestampUs(dateTime2);
 * 
 * // 格式化时间
 * QString formatted = GuiTime::formatTimestamp(timestampMs, "yyyy-MM-dd hh:mm:ss");
 * 
 * // 获取当前时间戳
 * int64_t currentMs = GuiTime::currentTimestampMs();
 * int64_t currentUs = GuiTime::currentTimestampUs();
 * int64_t currentNs = GuiTime::currentTimestampNs();
 */

} // namespace btra::gui