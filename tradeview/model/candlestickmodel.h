#ifndef CANDLESTICKMODEL_H
#define CANDLESTICKMODEL_H

#include <QAbstractListModel>
#include <QObject>
#include <QDateTime>
#include <QVector>

/**
 * @brief K线数据结构
 * 
 * 包含单根K线的所有数据信息，用于存储和传递K线数据
 */
struct CandlestickData {
  QDateTime timestamp;  ///< 时间戳
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
  CandlestickData() : open(0), high(0), low(0), close(0), volume(0), amount(0) {}

  /**
   * @brief 带参数构造函数
   * @param ts 时间戳
   * @param o 开盘价
   * @param h 最高价
   * @param l 最低价
   * @param c 收盘价
   * @param vol 成交量
   * @param amt 成交额
   */
  CandlestickData(const QDateTime& ts, double o, double h, double l, double c, qint64 vol, double amt)
      : timestamp(ts), open(o), high(h), low(l), close(c), volume(vol), amount(amt) {}
};

/**
 * @brief K线数据模型类
 * 
 * 继承自QAbstractListModel，用于管理K线图表数据
 * 支持实时更新、时间周期设置、价格范围统计等功能
 */
class CandlestickModel : public QAbstractListModel {
  Q_OBJECT

 public:
  /**
   * @brief 模型角色枚举
   *
   * 定义了QML中可以访问的数据角色
   */
  enum Roles {
    kTimestampRole = Qt::UserRole + 1,  ///< 时间戳角色
    kOpenRole,                          ///< 开盘价角色
    kHighRole,                          ///< 最高价角色
    kLowRole,                           ///< 最低价角色
    kCloseRole,                         ///< 收盘价角色
    kVolumeRole,                        ///< 成交量角色
    kAmountRole,                        ///< 成交额角色
    kIsUpRole,                          ///< 是否上涨角色
    kIsDownRole,                        ///< 是否下跌角色
    kBodyHeightRole,                    ///< 实体高度角色
    kUpperShadowRole,                   ///< 上影线长度角色
    kLowerShadowRole                    ///< 下影线长度角色
  };

  /**
   * @brief 构造函数
   * @param parent 父对象指针
   */
  explicit CandlestickModel(QObject* parent = nullptr);

  // QAbstractListModel interface
  /**
   * @brief 获取模型行数
   * @param parent 父索引
   * @return 行数
   */
  int rowCount(const QModelIndex& parent = QModelIndex()) const override;

  /**
   * @brief 获取指定索引和角色的数据
   * @param index 模型索引
   * @param role 数据角色
   * @return 数据值
   */
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

  /**
   * @brief 获取角色名称映射
   * @return 角色名称哈希表
   */
  QHash<int, QByteArray> roleNames() const override;

  // Public methods
  /**
   * @brief 添加K线数据
   * @param data K线数据
   */
  Q_INVOKABLE void add_candlestick(const CandlestickData& data);

  /**
   * @brief 更新K线数据
   * @param data K线数据
   */
  Q_INVOKABLE void update_candlestick(const CandlestickData& data);

  /**
   * @brief 清空所有数据
   */
  Q_INVOKABLE void clear();

  /**
   * @brief 设置时间周期
   * @param timeframe 时间周期字符串
   */
  Q_INVOKABLE void set_timeframe(const QString& timeframe);

  /**
   * @brief 获取时间周期
   * @return 时间周期字符串
   */
  Q_INVOKABLE QString get_timeframe() const;

  /**
   * @brief 获取K线数量
   * @return K线数量
   */
  Q_INVOKABLE int get_count() const;

  /**
   * @brief 获取指定索引的K线数据
   * @param index 索引
   * @return K线数据
   */
  Q_INVOKABLE CandlestickData get_candlestick(int index) const;

  /**
   * @brief 获取指定范围的K线数据
   * @param start 起始索引
   * @param count 数量
   * @return K线数据列表
   */
  Q_INVOKABLE QVariantList get_candlesticks(int start, int count) const;

 signals:
  /**
   * @brief 数据变化信号
   * 当模型数据发生变化时发出
   */
  void dataChanged();

  /**
   * @brief K线添加信号
   * 当添加新的K线数据时发出
   */
  void candlestickAdded();

  /**
   * @brief K线更新信号
   * @param index 更新的K线索引
   */
  void candlestickUpdated(int index);

 private:
  QVector<CandlestickData> m_candlesticks;  ///< K线数据容器
  QString m_timeframe;                       ///< 时间周期
  double m_max_price;                        ///< 最高价格
  double m_min_price;                        ///< 最低价格
  qint64 m_max_volume;                       ///< 最大成交量

  /**
   * @brief 更新价格范围
   * 重新计算最高价和最低价
   */
  void update_price_range();

  /**
   * @brief 更新成交量范围
   * 重新计算最大成交量
   */
  void update_volume_range();
};

#endif // CANDLESTICKMODEL_H 