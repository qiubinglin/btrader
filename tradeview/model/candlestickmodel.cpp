#include "candlestickmodel.h"

#include "guidb/kline.h"

namespace btra::gui {

/**
 * @brief 构造函数
 * @param parent 父对象指针
 */
CandlestickModel::CandlestickModel(const QString& name, KLineSetSPtr klineset, QObject* parent)
    : QAbstractListModel(parent), name_(name), klineset_(klineset) {
    klineset_->add_notice([this](NotifyType type) { emit this->dataChanged(); });
}

/**
 * @brief 获取模型行数
 * @param parent 父索引
 * @return K线数据的总数量
 */
int CandlestickModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid() or not klineset_) return 0;
    return klineset_->size();
}

/**
 * @brief 获取指定索引和角色的数据
 * @param index 模型索引
 * @param role 数据角色
 * @return 对应的数据值
 *
 * 根据不同的角色返回K线的不同属性值
 */
QVariant CandlestickModel::data(const QModelIndex& index, int role) const {
    if (!klineset_ || !index.isValid() || index.row() >= klineset_->count()) return QVariant();

    const auto& data = klineset_->at(index.row());

    switch (role) {
        case kStartTimeRole:
            return data.start_time; // 返回开始时间
        case kEndTimeRole:
            return data.end_time; // 返回结束时间
        case kOpenRole:
            return data.open; // 返回开盘价
        case kHighRole:
            return data.high; // 返回最高价
        case kLowRole:
            return data.low; // 返回最低价
        case kCloseRole:
            return data.close; // 返回收盘价
        case kVolumeRole:
            return data.volume; // 返回成交量
        case kAmountRole:
            return data.amount; // 返回成交额
        case kIsUpRole:
            return data.close >= data.open; // 判断是否上涨
        case kIsDownRole:
            return data.close < data.open; // 判断是否下跌
        case kBodyHeightRole:
            return qAbs(data.close - data.open); // 计算实体高度
        case kUpperShadowRole:
            return data.high - qMax(data.open, data.close); // 计算上影线长度
        case kLowerShadowRole:
            return qMin(data.open, data.close) - data.low; // 计算下影线长度
        default:
            return QVariant();
    }
}

/**
 * @brief 获取角色名称映射
 * @return 角色名称哈希表
 *
 * 定义QML中可以访问的属性名称
 */
QHash<int, QByteArray> CandlestickModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[kStartTimeRole] = "start_time";    // 开始时间
    roles[kEndTimeRole] = "end_time";        // 结束时间
    roles[kOpenRole] = "open";               // 开盘价
    roles[kHighRole] = "high";               // 最高价
    roles[kLowRole] = "low";                 // 最低价
    roles[kCloseRole] = "close";             // 收盘价
    roles[kVolumeRole] = "volume";           // 成交量
    roles[kAmountRole] = "amount";           // 成交额
    roles[kIsUpRole] = "isUp";               // 是否上涨
    roles[kIsDownRole] = "isDown";           // 是否下跌
    roles[kBodyHeightRole] = "bodyHeight";   // 实体高度
    roles[kUpperShadowRole] = "upperShadow"; // 上影线长度
    roles[kLowerShadowRole] = "lowerShadow"; // 下影线长度
    return roles;
}

/**
 * @brief 获取时间周期
 * @return 时间周期字符串
 */
QString CandlestickModel::get_timeframe() const {
    if (klineset_) {
        return klineset_->get_timeframe();
    }
    return "";
}

/**
 * @brief 获取K线数量
 * @return K线数量
 */
int CandlestickModel::get_count() const {
    if (klineset_) return klineset_->count();
    return 0;
}

/**
 * @brief 获取指定范围的K线数据
 * @param start 起始索引
 * @param count 数量
 * @return K线数据列表
 */
QVariantList CandlestickModel::get_candlesticks(int start, int count) const {
    QVariantList result;
    if (not klineset_) return result;
    int end = qMin(start + count, klineset_->count());

    for (int i = start; i < end; ++i) {
        QVariantMap candlestick;
        const KLine& data = klineset_->at(i);
        candlestick["timestamp"] = data.start_time;
        candlestick["open"] = data.open;
        candlestick["high"] = data.high;
        candlestick["low"] = data.low;
        candlestick["close"] = data.close;
        candlestick["volume"] = data.volume;
        result.append(candlestick);
    }

    return result;
}

/**
 * @brief 获取指定索引的K线数据（QML友好接口）
 * @param index 索引
 * @return K线数据对象
 */
QVariantMap CandlestickModel::get(int index) const {
    QVariantMap result;
    if (not klineset_) return result;
    if (index >= 0 && index < klineset_->count()) {
        const KLine& data = klineset_->at(index);
        result["timestamp"] = data.start_time;
        result["open"] = data.open;
        result["high"] = data.high;
        result["low"] = data.low;
        result["close"] = data.close;
        result["volume"] = data.volume;
    }
    return result;
}

void CandlestickModel::setDisplayContent(const QVariantMap& indication) {}

QString CandlestickModel::get_name() const { return name_; }

} // namespace btra::gui