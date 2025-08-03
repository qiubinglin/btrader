#ifndef MICROORDERBOOKMODEL_H
#define MICROORDERBOOKMODEL_H

#include <QAbstractListModel>
#include <QObject>
#include <QDateTime>
#include <QVector>

namespace btra::gui {

/**
 * @brief 微盘口档位数据结构
 * 
 * 包含微盘口中单个价格档位的信息，支持高亮显示
 */
struct MicroOrderBookLevel {
    double price;         ///< 价格档位
    qint64 volume;        ///< 挂单量
    int orderCount;       ///< 订单数量
    QString side;         ///< 买卖方向 ("bid" 或 "ask")
    double percentage;    ///< 占总量的百分比
    bool isHighlighted;   ///< 是否高亮显示（用于突出重要档位）
    
    /**
     * @brief 默认构造函数
     * 初始化所有成员变量为0或false
     */
    MicroOrderBookLevel() : price(0), volume(0), orderCount(0), percentage(0), isHighlighted(false) {}
    
    /**
     * @brief 带参数构造函数
     * @param p 价格档位
     * @param vol 挂单量
     * @param count 订单数量
     * @param s 买卖方向
     */
    MicroOrderBookLevel(double p, qint64 vol, int count, const QString& s)
        : price(p), volume(vol), orderCount(count), side(s), percentage(0), isHighlighted(false) {}
};

class MicroOrderBookModel : public QAbstractListModel
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
        PercentageRole,
        IsHighlightedRole,
        CumulativeVolumeRole,
        DepthColorRole
    };

    explicit MicroOrderBookModel(QObject *parent = nullptr);

    // QAbstractListModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    // Public methods
    Q_INVOKABLE void updateMicroOrderBook(const QVector<MicroOrderBookLevel> &bids, const QVector<MicroOrderBookLevel> &asks);
    Q_INVOKABLE void clear();
    Q_INVOKABLE void setMaxLevels(int levels);
    Q_INVOKABLE int getMaxLevels() const;
    Q_INVOKABLE void setHighlightThreshold(double threshold);
    Q_INVOKABLE double getHighlightThreshold() const;
    Q_INVOKABLE double getBestBid() const;
    Q_INVOKABLE double getBestAsk() const;
    Q_INVOKABLE double getSpread() const;
    Q_INVOKABLE double getMidPrice() const;
    Q_INVOKABLE QVariantList getBids() const;
    Q_INVOKABLE QVariantList getAsks() const;
    Q_INVOKABLE QVariantList getMicroOrderBook() const;
    Q_INVOKABLE void setVolumeThreshold(qint64 threshold);
    Q_INVOKABLE qint64 getVolumeThreshold() const;

signals:
    void dataChanged();
    void microOrderBookUpdated();
    void bestBidChanged(double price);
    void bestAskChanged(double price);
    void spreadChanged(double spread);
    void volumeThresholdChanged(qint64 threshold);

private:
    QVector<MicroOrderBookLevel> m_bids;
    QVector<MicroOrderBookLevel> m_asks;
    QVector<MicroOrderBookLevel> m_combinedOrderBook;
    int m_maxLevels;
    double m_bestBid;
    double m_bestAsk;
    double m_highlightThreshold;
    qint64 m_volumeThreshold;
    qint64 m_maxVolume;

    void updateCombinedOrderBook();
    void updateBestPrices();
    void calculatePercentages();
    void updateHighlighting();
    qint64 calculateCumulativeVolume(int index) const;
    QString getDepthColor(int index) const;
};

}

#endif // MICROORDERBOOKMODEL_H 