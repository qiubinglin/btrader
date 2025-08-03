#ifndef ORDERBOOKMODEL_H
#define ORDERBOOKMODEL_H

#include <QAbstractListModel>
#include <QObject>
#include <QDateTime>
#include <QVector>

/**
 * @brief 订单簿档位数据结构
 * 
 * 包含订单簿中单个价格档位的信息
 */
struct OrderBookLevel {
    double price;      ///< 价格档位
    qint64 volume;     ///< 挂单量
    int orderCount;    ///< 订单数量
    QString side;      ///< 买卖方向 ("bid" 或 "ask")
    
    /**
     * @brief 默认构造函数
     * 初始化所有成员变量为0
     */
    OrderBookLevel() : price(0), volume(0), orderCount(0) {}
    
    /**
     * @brief 带参数构造函数
     * @param p 价格档位
     * @param vol 挂单量
     * @param count 订单数量
     * @param s 买卖方向
     */
    OrderBookLevel(double p, qint64 vol, int count, const QString& s)
        : price(p), volume(vol), orderCount(count), side(s) {}
};

class OrderBookModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        PriceRole = Qt::UserRole + 1,
        VolumeRole,
        OrderCountRole,
        SideRole,
        IsBidRole,
        IsAskRole,
        CumulativeVolumeRole,
        DepthPercentageRole
    };

    explicit OrderBookModel(QObject *parent = nullptr);

    // QAbstractListModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    // Public methods
    Q_INVOKABLE void updateOrderBook(const QVector<OrderBookLevel> &bids, const QVector<OrderBookLevel> &asks);
    Q_INVOKABLE void clear();
    Q_INVOKABLE void setMaxLevels(int levels);
    Q_INVOKABLE int getMaxLevels() const;
    Q_INVOKABLE double getBestBid() const;
    Q_INVOKABLE double getBestAsk() const;
    Q_INVOKABLE double getSpread() const;
    Q_INVOKABLE double getMidPrice() const;
    Q_INVOKABLE QVariantList getBids() const;
    Q_INVOKABLE QVariantList getAsks() const;
    Q_INVOKABLE QVariantList getOrderBook() const;

signals:
    void dataChanged();
    void orderBookUpdated();
    void bestBidChanged(double price);
    void bestAskChanged(double price);
    void spreadChanged(double spread);

private:
    QVector<OrderBookLevel> m_bids;
    QVector<OrderBookLevel> m_asks;
    QVector<OrderBookLevel> m_combinedOrderBook;
    int m_maxLevels;
    double m_bestBid;
    double m_bestAsk;

    void updateCombinedOrderBook();
    void updateBestPrices();
    qint64 calculateCumulativeVolume(int index) const;
    double calculateDepthPercentage(int index) const;
};

#endif // ORDERBOOKMODEL_H 