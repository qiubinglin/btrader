#ifndef TICKTRADEMODEL_H
#define TICKTRADEMODEL_H

#include <QAbstractListModel>
#include <QObject>
#include <QDateTime>
#include <QVector>

/**
 * @brief 逐笔成交数据结构
 * 
 * 包含单笔成交的所有信息，用于存储和传递逐笔成交数据
 */
struct TickTradeData {
    QDateTime timestamp;  ///< 成交时间戳
    double price;         ///< 成交价格
    qint64 volume;        ///< 成交数量
    QString direction;    ///< 成交方向 ("buy" 或 "sell")
    QString tradeId;      ///< 成交ID
    QString symbol;       ///< 交易对符号
    
    /**
     * @brief 默认构造函数
     * 初始化价格和成交量为0
     */
    TickTradeData() : price(0), volume(0) {}
    
    /**
     * @brief 带参数构造函数
     * @param ts 时间戳
     * @param p 成交价格
     * @param vol 成交数量
     * @param dir 成交方向
     * @param id 成交ID
     * @param sym 交易对符号
     */
    TickTradeData(const QDateTime& ts, double p, qint64 vol, const QString& dir, 
                  const QString& id, const QString& sym)
        : timestamp(ts), price(p), volume(vol), direction(dir), tradeId(id), symbol(sym) {}
};

class TickTradeModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        TimestampRole = Qt::UserRole + 1,
        PriceRole,
        VolumeRole,
        DirectionRole,
        TradeIdRole,
        SymbolRole,
        IsBuyRole,
        IsSellRole,
        AmountRole
    };

    explicit TickTradeModel(QObject *parent = nullptr);

    // QAbstractListModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    // Public methods
    Q_INVOKABLE void addTickTrade(const TickTradeData &data);
    Q_INVOKABLE void clear();
    Q_INVOKABLE void setMaxCount(int count);
    Q_INVOKABLE int getMaxCount() const;
    Q_INVOKABLE int getCount() const;
    Q_INVOKABLE TickTradeData getTickTrade(int index) const;
    Q_INVOKABLE QVariantList getTickTrades(int start, int count) const;
    Q_INVOKABLE double getLatestPrice() const;
    Q_INVOKABLE qint64 getTotalVolume() const;

signals:
    void dataChanged();
    void tickTradeAdded();
    void latestPriceChanged(double price);

private:
    QVector<TickTradeData> m_tickTrades;
    int m_maxCount;
    double m_latestPrice;
    qint64 m_totalVolume;

    void updateLatestPrice();
    void updateTotalVolume();
};

#endif // TICKTRADEMODEL_H 