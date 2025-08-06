#ifndef CANDLESTICKMODEL_H
#define CANDLESTICKMODEL_H

#include <QAbstractListModel>
#include <QDateTime>
#include <QObject>
#include <QVector>

#include "guidb/klineset.h"

namespace btra::gui {

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
        kStartTimeRole = Qt::UserRole + 1, ///< 开始时间角色
        kEndTimeRole,                      ///< 结束时间角色
        kOpenRole,                         ///< 开盘价角色
        kHighRole,                         ///< 最高价角色
        kLowRole,                          ///< 最低价角色
        kCloseRole,                        ///< 收盘价角色
        kVolumeRole,                       ///< 成交量角色
        kAmountRole,                       ///< 成交额角色
        kIsUpRole,                         ///< 是否上涨角色
        kIsDownRole,                       ///< 是否下跌角色
        kBodyHeightRole,                   ///< 实体高度角色
        kUpperShadowRole,                  ///< 上影线长度角色
        kLowerShadowRole                   ///< 下影线长度角色
    };

    explicit CandlestickModel(const QString &name, KLineSetSPtr klineset, QObject* parent = nullptr);

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
     * @brief K线数量属性（QML友好）
     * @return K线数量
     */
    Q_PROPERTY(int count READ get_count NOTIFY dataChanged)

    /**
     * @brief 获取指定范围的K线数据
     * @param start 起始索引
     * @param count 数量
     * @return K线数据列表
     */
    Q_INVOKABLE QVariantList get_candlesticks(int start, int count) const;

    /**
     * @brief 获取指定索引的K线数据（QML友好接口）
     * @param index 索引
     * @return K线数据对象
     */
    Q_INVOKABLE QVariantMap get(int index) const;

    Q_INVOKABLE void setDisplayContent(const QVariantMap& indication);

    Q_INVOKABLE QString get_name() const;

    void debug_add_kline(const KLine &kline) {
        klineset_->push_back(kline);
    }

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
    QString name_;
    KLineSetSPtr klineset_{nullptr};
};

} // namespace btra::gui

#endif // CANDLESTICKMODEL_H