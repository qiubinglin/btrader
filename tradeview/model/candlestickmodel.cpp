#include "candlestickmodel.h"
#include <QDebug>

/**
 * @brief 构造函数
 * @param parent 父对象指针
 *
 * 初始化K线模型，设置默认时间周期为1分钟
 */
CandlestickModel::CandlestickModel(QObject* parent)
    : QAbstractListModel(parent),
      m_timeframe("1m"),      // 默认时间周期为1分钟
      m_max_price(0),         // 初始化最高价格为0
      m_min_price(0),         // 初始化最低价格为0
      m_max_volume(0) {       // 初始化最大成交量为0
}

/**
 * @brief 获取模型行数
 * @param parent 父索引
 * @return K线数据的总数量
 */
int CandlestickModel::rowCount(const QModelIndex& parent) const {
  if (parent.isValid()) return 0;
  return m_candlesticks.size();
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
  if (!index.isValid() || index.row() >= m_candlesticks.size())
    return QVariant();

  const CandlestickData& data = m_candlesticks[index.row()];

  switch (role) {
    case kStartTimeRole:
      return data.start_time;                   // 返回开始时间
    case kEndTimeRole:
      return data.end_time;                     // 返回结束时间
    case kOpenRole:
      return data.open;                         // 返回开盘价
    case kHighRole:
      return data.high;                         // 返回最高价
    case kLowRole:
      return data.low;                          // 返回最低价
    case kCloseRole:
      return data.close;                        // 返回收盘价
    case kVolumeRole:
      return data.volume;                       // 返回成交量
    case kAmountRole:
      return data.amount;                       // 返回成交额
    case kIsUpRole:
      return data.close >= data.open;           // 判断是否上涨
    case kIsDownRole:
      return data.close < data.open;            // 判断是否下跌
    case kBodyHeightRole:
      return qAbs(data.close - data.open);      // 计算实体高度
    case kUpperShadowRole:
      return data.high - qMax(data.open, data.close);  // 计算上影线长度
    case kLowerShadowRole:
      return qMin(data.open, data.close) - data.low;   // 计算下影线长度
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
  roles[kStartTimeRole] = "start_time";     // 开始时间
  roles[kEndTimeRole] = "end_time";         // 结束时间
  roles[kOpenRole] = "open";                // 开盘价
  roles[kHighRole] = "high";                // 最高价
  roles[kLowRole] = "low";                  // 最低价
  roles[kCloseRole] = "close";              // 收盘价
  roles[kVolumeRole] = "volume";            // 成交量
  roles[kAmountRole] = "amount";            // 成交额
  roles[kIsUpRole] = "isUp";                // 是否上涨
  roles[kIsDownRole] = "isDown";            // 是否下跌
  roles[kBodyHeightRole] = "bodyHeight";    // 实体高度
  roles[kUpperShadowRole] = "upperShadow";  // 上影线长度
  roles[kLowerShadowRole] = "lowerShadow";  // 下影线长度
  return roles;
}

/**
 * @brief 添加K线数据
 * @param data K线数据
 */
void CandlestickModel::add_candlestick(const CandlestickData& data) {
  beginInsertRows(QModelIndex(), m_candlesticks.size(), m_candlesticks.size());
  m_candlesticks.append(data);
  endInsertRows();

  update_price_range();
  update_volume_range();

  emit dataChanged();
  emit candlestickAdded();
}

/**
 * @brief 更新K线数据
 * @param data K线数据
 */
void CandlestickModel::update_candlestick(const CandlestickData& data) {
  // Find the candlestick with the same end_time
  for (int i = 0; i < m_candlesticks.size(); ++i) {
    if (m_candlesticks[i].end_time == data.end_time) {
      m_candlesticks[i] = data;
      QModelIndex index = createIndex(i, 0);
      emit QAbstractListModel::dataChanged(index, index);
      emit candlestickUpdated(i);
      break;
    }
  }

  update_price_range();
  update_volume_range();
}

/**
 * @brief 清空所有数据
 */
void CandlestickModel::clear() {
  if (m_candlesticks.isEmpty()) return;

  beginResetModel();
  m_candlesticks.clear();
  endResetModel();

  m_max_price = 0;
  m_min_price = 0;
  m_max_volume = 0;

  emit dataChanged();
}

/**
 * @brief 设置时间周期
 * @param timeframe 时间周期字符串
 */
void CandlestickModel::set_timeframe(const QString& timeframe) {
  if (m_timeframe != timeframe) {
    m_timeframe = timeframe;
    emit dataChanged();
  }
}

/**
 * @brief 获取时间周期
 * @return 时间周期字符串
 */
QString CandlestickModel::get_timeframe() const {
  return m_timeframe;
}

/**
 * @brief 获取K线数量
 * @return K线数量
 */
int CandlestickModel::get_count() const {
  return m_candlesticks.size();
}

/**
 * @brief 获取指定索引的K线数据
 * @param index 索引
 * @return K线数据
 */
CandlestickData CandlestickModel::get_candlestick(int index) const {
  if (index >= 0 && index < m_candlesticks.size()) {
    return m_candlesticks[index];
  }
  return CandlestickData();
}

/**
 * @brief 获取指定范围的K线数据
 * @param start 起始索引
 * @param count 数量
 * @return K线数据列表
 */
QVariantList CandlestickModel::get_candlesticks(int start, int count) const {
  QVariantList result;
  int end = qMin(start + count, m_candlesticks.size());

  for (int i = start; i < end; ++i) {
    QVariantMap candlestick;
    const CandlestickData& data = m_candlesticks[i];
    candlestick["start_time"] = data.start_time;
    candlestick["end_time"] = data.end_time;
    candlestick["open"] = data.open;
    candlestick["high"] = data.high;
    candlestick["low"] = data.low;
    candlestick["close"] = data.close;
    candlestick["volume"] = data.volume;
    candlestick["amount"] = data.amount;
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
  if (index >= 0 && index < m_candlesticks.size()) {
    const CandlestickData& data = m_candlesticks[index];
    result["start_time"] = data.start_time;
    result["end_time"] = data.end_time;
    result["open"] = data.open;
    result["high"] = data.high;
    result["low"] = data.low;
    result["close"] = data.close;
    result["volume"] = data.volume;
    result["amount"] = data.amount;
  }
  return result;
}

/**
 * @brief 更新价格范围
 * 重新计算最高价和最低价
 */
void CandlestickModel::update_price_range() {
  if (m_candlesticks.isEmpty()) return;

  m_max_price = m_candlesticks.first().high;
  m_min_price = m_candlesticks.first().low;

  for (const auto& data : m_candlesticks) {
    m_max_price = qMax(m_max_price, data.high);
    m_min_price = qMin(m_min_price, data.low);
  }
}

/**
 * @brief 更新成交量范围
 * 重新计算最大成交量
 */
void CandlestickModel::update_volume_range() {
  if (m_candlesticks.isEmpty()) return;

  m_max_volume = m_candlesticks.first().volume;

  for (const auto& data : m_candlesticks) {
    m_max_volume = qMax(m_max_volume, data.volume);
  }
} 