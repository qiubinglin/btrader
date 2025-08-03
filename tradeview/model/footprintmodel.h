#ifndef FOOTPRINTMODEL_H
#define FOOTPRINTMODEL_H

#include <QAbstractListModel>
#include <QObject>
#include <QDateTime>
#include <QVector>
#include <QMap>

namespace btra::gui {

/**
 * @brief 足迹图单元格数据结构
 * 
 * 包含足迹图中单个价格单元格的买卖压力信息
 */
struct FootprintCell {
    double price;         ///< 价格档位
    qint64 buyVolume;     ///< 买入成交量
    qint64 sellVolume;    ///< 卖出成交量
    qint64 totalVolume;   ///< 总成交量
    int buyCount;         ///< 买入笔数
    int sellCount;        ///< 卖出笔数
    int totalCount;       ///< 总笔数
    double delta;         ///< Delta值 (买入成交量 - 卖出成交量)
    double deltaPercent;  ///< Delta百分比 (delta / totalVolume * 100)
    
    /**
     * @brief 默认构造函数
     * 初始化所有成员变量为0
     */
    FootprintCell() : price(0), buyVolume(0), sellVolume(0), totalVolume(0), 
                      buyCount(0), sellCount(0), totalCount(0), delta(0), deltaPercent(0) {}
    
    /**
     * @brief 带价格参数的构造函数
     * @param p 价格档位
     */
    FootprintCell(double p) : price(p), buyVolume(0), sellVolume(0), totalVolume(0),
                              buyCount(0), sellCount(0), totalCount(0), delta(0), deltaPercent(0) {}
};

/**
 * @brief 足迹图K线数据结构
 * 
 * 包含足迹图中单根K线的OHLC信息和价格单元格数据
 */
struct FootprintBar {
    QDateTime timestamp;              ///< 时间戳
    double open;                      ///< 开盘价
    double high;                      ///< 最高价
    double low;                       ///< 最低价
    double close;                     ///< 收盘价
    QVector<FootprintCell> cells;     ///< 价格单元格数组
    
    /**
     * @brief 默认构造函数
     * 初始化OHLC为0
     */
    FootprintBar() : open(0), high(0), low(0), close(0) {}
    
    /**
     * @brief 带参数构造函数
     * @param ts 时间戳
     * @param o 开盘价
     * @param h 最高价
     * @param l 最低价
     * @param c 收盘价
     */
    FootprintBar(const QDateTime& ts, double o, double h, double l, double c)
        : timestamp(ts), open(o), high(h), low(l), close(c) {}
};

class FootprintModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        TimestampRole = Qt::UserRole + 1,
        OpenRole,
        HighRole,
        LowRole,
        CloseRole,
        IsUpRole,
        IsDownRole,
        CellsRole,
        TotalBuyVolumeRole,
        TotalSellVolumeRole,
        TotalVolumeRole,
        DeltaRole,
        DeltaPercentRole
    };

    explicit FootprintModel(QObject *parent = nullptr);

    // QAbstractListModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    // Public methods
    Q_INVOKABLE void addFootprintBar(const FootprintBar &bar);
    Q_INVOKABLE void updateFootprintBar(const FootprintBar &bar);
    Q_INVOKABLE void addTickData(const QDateTime &timestamp, double price, qint64 volume, const QString &direction);
    Q_INVOKABLE void clear();
    Q_INVOKABLE void setTimeframe(const QString &timeframe);
    Q_INVOKABLE QString getTimeframe() const;
    Q_INVOKABLE void setPriceStep(double step);
    Q_INVOKABLE double getPriceStep() const;
    Q_INVOKABLE int getCount() const;
    Q_INVOKABLE FootprintBar getFootprintBar(int index) const;
    Q_INVOKABLE QVariantList getFootprintBars(int start, int count) const;

signals:
    void dataChanged();
    void footprintBarAdded();
    void footprintBarUpdated(int index);

private:
    QVector<FootprintBar> m_footprintBars;
    QString m_timeframe;
    double m_priceStep;
    QMap<QDateTime, FootprintBar> m_currentBarMap;
    
    void updateCurrentBar(const QDateTime &timestamp, double price, qint64 volume, const QString &direction);
    QDateTime getBarStartTime(const QDateTime &timestamp) const;
    double roundToPriceStep(double price) const;
    void calculateBarStatistics(FootprintBar &bar);
    qint64 calculateTotalBuyVolume(const FootprintBar &bar) const;
    qint64 calculateTotalSellVolume(const FootprintBar &bar) const;
    qint64 calculateTotalVolume(const FootprintBar &bar) const;
    double calculateDelta(const FootprintBar &bar) const;
    double calculateDeltaPercent(const FootprintBar &bar) const;
};

}

#endif // FOOTPRINTMODEL_H 