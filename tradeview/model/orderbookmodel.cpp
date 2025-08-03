#include "orderbookmodel.h"
#include <QDebug>
#include <algorithm>

OrderBookModel::OrderBookModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_maxLevels(20)
    , m_bestBid(0)
    , m_bestAsk(0)
{
}

int OrderBookModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_combinedOrderBook.size();
}

QVariant OrderBookModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_combinedOrderBook.size())
        return QVariant();

    const OrderBookLevel &level = m_combinedOrderBook[index.row()];

    switch (role) {
    case PriceRole:
        return level.price;
    case VolumeRole:
        return level.volume;
    case OrderCountRole:
        return level.orderCount;
    case SideRole:
        return level.side;
    case IsBidRole:
        return level.side == "bid";
    case IsAskRole:
        return level.side == "ask";
    case CumulativeVolumeRole:
        return calculateCumulativeVolume(index.row());
    case DepthPercentageRole:
        return calculateDepthPercentage(index.row());
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> OrderBookModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[PriceRole] = "price";
    roles[VolumeRole] = "volume";
    roles[OrderCountRole] = "orderCount";
    roles[SideRole] = "side";
    roles[IsBidRole] = "isBid";
    roles[IsAskRole] = "isAsk";
    roles[CumulativeVolumeRole] = "cumulativeVolume";
    roles[DepthPercentageRole] = "depthPercentage";
    return roles;
}

void OrderBookModel::updateOrderBook(const QVector<OrderBookLevel> &bids, const QVector<OrderBookLevel> &asks)
{
    m_bids = bids;
    m_asks = asks;

    // Sort bids in descending order (highest first)
    std::sort(m_bids.begin(), m_bids.end(), [](const OrderBookLevel &a, const OrderBookLevel &b) {
        return a.price > b.price;
    });

    // Sort asks in ascending order (lowest first)
    std::sort(m_asks.begin(), m_asks.end(), [](const OrderBookLevel &a, const OrderBookLevel &b) {
        return a.price < b.price;
    });

    // Limit to max levels
    if (m_bids.size() > m_maxLevels) {
        m_bids.resize(m_maxLevels);
    }
    if (m_asks.size() > m_maxLevels) {
        m_asks.resize(m_maxLevels);
    }

    updateCombinedOrderBook();
    updateBestPrices();

    beginResetModel();
    endResetModel();

    emit dataChanged();
    emit orderBookUpdated();
}

void OrderBookModel::clear()
{
    beginResetModel();
    m_bids.clear();
    m_asks.clear();
    m_combinedOrderBook.clear();
    endResetModel();

    m_bestBid = 0;
    m_bestAsk = 0;

    emit dataChanged();
}

void OrderBookModel::setMaxLevels(int levels)
{
    if (m_maxLevels != levels) {
        m_maxLevels = levels;
        
        // Resize existing data
        if (m_bids.size() > m_maxLevels) {
            m_bids.resize(m_maxLevels);
        }
        if (m_asks.size() > m_maxLevels) {
            m_asks.resize(m_maxLevels);
        }
        
        updateCombinedOrderBook();
        beginResetModel();
        endResetModel();
        emit dataChanged();
    }
}

int OrderBookModel::getMaxLevels() const
{
    return m_maxLevels;
}

double OrderBookModel::getBestBid() const
{
    return m_bestBid;
}

double OrderBookModel::getBestAsk() const
{
    return m_bestAsk;
}

double OrderBookModel::getSpread() const
{
    if (m_bestBid > 0 && m_bestAsk > 0) {
        return m_bestAsk - m_bestBid;
    }
    return 0;
}

double OrderBookModel::getMidPrice() const
{
    if (m_bestBid > 0 && m_bestAsk > 0) {
        return (m_bestBid + m_bestAsk) / 2.0;
    }
    return 0;
}

QVariantList OrderBookModel::getBids() const
{
    QVariantList result;
    for (const auto &bid : m_bids) {
        QVariantMap level;
        level["price"] = bid.price;
        level["volume"] = bid.volume;
        level["orderCount"] = bid.orderCount;
        level["side"] = bid.side;
        result.append(level);
    }
    return result;
}

QVariantList OrderBookModel::getAsks() const
{
    QVariantList result;
    for (const auto &ask : m_asks) {
        QVariantMap level;
        level["price"] = ask.price;
        level["volume"] = ask.volume;
        level["orderCount"] = ask.orderCount;
        level["side"] = ask.side;
        result.append(level);
    }
    return result;
}

QVariantList OrderBookModel::getOrderBook() const
{
    QVariantList result;
    for (const auto &level : m_combinedOrderBook) {
        QVariantMap orderBookLevel;
        orderBookLevel["price"] = level.price;
        orderBookLevel["volume"] = level.volume;
        orderBookLevel["orderCount"] = level.orderCount;
        orderBookLevel["side"] = level.side;
        result.append(orderBookLevel);
    }
    return result;
}

void OrderBookModel::updateCombinedOrderBook()
{
    m_combinedOrderBook.clear();
    
    // Add asks (in reverse order for display)
    for (int i = m_asks.size() - 1; i >= 0; --i) {
        m_combinedOrderBook.append(m_asks[i]);
    }
    
    // Add bids
    for (const auto &bid : m_bids) {
        m_combinedOrderBook.append(bid);
    }
}

void OrderBookModel::updateBestPrices()
{
    double oldBestBid = m_bestBid;
    double oldBestAsk = m_bestAsk;
    
    m_bestBid = m_bids.isEmpty() ? 0 : m_bids.first().price;
    m_bestAsk = m_asks.isEmpty() ? 0 : m_asks.first().price;
    
    if (oldBestBid != m_bestBid) {
        emit bestBidChanged(m_bestBid);
    }
    if (oldBestAsk != m_bestAsk) {
        emit bestAskChanged(m_bestAsk);
    }
    
    double spread = getSpread();
    if (spread > 0) {
        emit spreadChanged(spread);
    }
}

qint64 OrderBookModel::calculateCumulativeVolume(int index) const
{
    if (index < 0 || index >= m_combinedOrderBook.size()) {
        return 0;
    }
    
    qint64 cumulative = 0;
    const QString &side = m_combinedOrderBook[index].side;
    
    if (side == "ask") {
        // For asks, sum from current level to best ask
        for (int i = index; i < m_combinedOrderBook.size() && m_combinedOrderBook[i].side == "ask"; ++i) {
            cumulative += m_combinedOrderBook[i].volume;
        }
    } else if (side == "bid") {
        // For bids, sum from current level to best bid
        for (int i = index; i >= 0 && m_combinedOrderBook[i].side == "bid"; --i) {
            cumulative += m_combinedOrderBook[i].volume;
        }
    }
    
    return cumulative;
}

double OrderBookModel::calculateDepthPercentage(int index) const
{
    if (index < 0 || index >= m_combinedOrderBook.size()) {
        return 0;
    }
    
    qint64 totalVolume = 0;
    const QString &side = m_combinedOrderBook[index].side;
    
    // Calculate total volume for the side
    for (const auto &level : m_combinedOrderBook) {
        if (level.side == side) {
            totalVolume += level.volume;
        }
    }
    
    if (totalVolume == 0) {
        return 0;
    }
    
    return (double)m_combinedOrderBook[index].volume / totalVolume * 100.0;
} 