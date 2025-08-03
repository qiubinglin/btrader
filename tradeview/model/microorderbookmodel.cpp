#include "microorderbookmodel.h"
#include <QDebug>
#include <algorithm>

namespace btra::gui {

MicroOrderBookModel::MicroOrderBookModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_maxLevels(10)
    , m_bestBid(0)
    , m_bestAsk(0)
    , m_highlightThreshold(0.1)  // 10%
    , m_volumeThreshold(1000)
    , m_maxVolume(0)
{
}

int MicroOrderBookModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_combinedOrderBook.size();
}

QVariant MicroOrderBookModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_combinedOrderBook.size())
        return QVariant();

    const MicroOrderBookLevel &level = m_combinedOrderBook[index.row()];

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
    case PercentageRole:
        return level.percentage;
    case IsHighlightedRole:
        return level.isHighlighted;
    case CumulativeVolumeRole:
        return calculateCumulativeVolume(index.row());
    case DepthColorRole:
        return getDepthColor(index.row());
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> MicroOrderBookModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[PriceRole] = "price";
    roles[VolumeRole] = "volume";
    roles[OrderCountRole] = "orderCount";
    roles[SideRole] = "side";
    roles[IsBidRole] = "isBid";
    roles[IsAskRole] = "isAsk";
    roles[PercentageRole] = "percentage";
    roles[IsHighlightedRole] = "isHighlighted";
    roles[CumulativeVolumeRole] = "cumulativeVolume";
    roles[DepthColorRole] = "depthColor";
    return roles;
}

void MicroOrderBookModel::updateMicroOrderBook(const QVector<MicroOrderBookLevel> &bids, const QVector<MicroOrderBookLevel> &asks)
{
    m_bids = bids;
    m_asks = asks;

    // Sort bids in descending order (highest first)
    std::sort(m_bids.begin(), m_bids.end(), [](const MicroOrderBookLevel &a, const MicroOrderBookLevel &b) {
        return a.price > b.price;
    });

    // Sort asks in ascending order (lowest first)
    std::sort(m_asks.begin(), m_asks.end(), [](const MicroOrderBookLevel &a, const MicroOrderBookLevel &b) {
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
    calculatePercentages();
    updateHighlighting();

    beginResetModel();
    endResetModel();

    emit dataChanged();
    emit microOrderBookUpdated();
}

void MicroOrderBookModel::clear()
{
    beginResetModel();
    m_bids.clear();
    m_asks.clear();
    m_combinedOrderBook.clear();
    endResetModel();

    m_bestBid = 0;
    m_bestAsk = 0;
    m_maxVolume = 0;

    emit dataChanged();
}

void MicroOrderBookModel::setMaxLevels(int levels)
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

int MicroOrderBookModel::getMaxLevels() const
{
    return m_maxLevels;
}

void MicroOrderBookModel::setHighlightThreshold(double threshold)
{
    if (m_highlightThreshold != threshold) {
        m_highlightThreshold = threshold;
        updateHighlighting();
        emit dataChanged();
    }
}

double MicroOrderBookModel::getHighlightThreshold() const
{
    return m_highlightThreshold;
}

double MicroOrderBookModel::getBestBid() const
{
    return m_bestBid;
}

double MicroOrderBookModel::getBestAsk() const
{
    return m_bestAsk;
}

double MicroOrderBookModel::getSpread() const
{
    if (m_bestBid > 0 && m_bestAsk > 0) {
        return m_bestAsk - m_bestBid;
    }
    return 0;
}

double MicroOrderBookModel::getMidPrice() const
{
    if (m_bestBid > 0 && m_bestAsk > 0) {
        return (m_bestBid + m_bestAsk) / 2.0;
    }
    return 0;
}

QVariantList MicroOrderBookModel::getBids() const
{
    QVariantList result;
    for (const auto &bid : m_bids) {
        QVariantMap level;
        level["price"] = bid.price;
        level["volume"] = bid.volume;
        level["orderCount"] = bid.orderCount;
        level["side"] = bid.side;
        level["percentage"] = bid.percentage;
        level["isHighlighted"] = bid.isHighlighted;
        result.append(level);
    }
    return result;
}

QVariantList MicroOrderBookModel::getAsks() const
{
    QVariantList result;
    for (const auto &ask : m_asks) {
        QVariantMap level;
        level["price"] = ask.price;
        level["volume"] = ask.volume;
        level["orderCount"] = ask.orderCount;
        level["side"] = ask.side;
        level["percentage"] = ask.percentage;
        level["isHighlighted"] = ask.isHighlighted;
        result.append(level);
    }
    return result;
}

QVariantList MicroOrderBookModel::getMicroOrderBook() const
{
    QVariantList result;
    for (const auto &level : m_combinedOrderBook) {
        QVariantMap orderBookLevel;
        orderBookLevel["price"] = level.price;
        orderBookLevel["volume"] = level.volume;
        orderBookLevel["orderCount"] = level.orderCount;
        orderBookLevel["side"] = level.side;
        orderBookLevel["percentage"] = level.percentage;
        orderBookLevel["isHighlighted"] = level.isHighlighted;
        result.append(orderBookLevel);
    }
    return result;
}

void MicroOrderBookModel::setVolumeThreshold(qint64 threshold)
{
    if (m_volumeThreshold != threshold) {
        m_volumeThreshold = threshold;
        updateHighlighting();
        emit volumeThresholdChanged(m_volumeThreshold);
        emit dataChanged();
    }
}

qint64 MicroOrderBookModel::getVolumeThreshold() const
{
    return m_volumeThreshold;
}

void MicroOrderBookModel::updateCombinedOrderBook()
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

void MicroOrderBookModel::updateBestPrices()
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

void MicroOrderBookModel::calculatePercentages()
{
    // Calculate total volume for percentage calculation
    qint64 totalVolume = 0;
    for (const auto &level : m_combinedOrderBook) {
        totalVolume += level.volume;
    }
    
    if (totalVolume == 0) return;
    
    // Update percentages
    for (auto &level : m_combinedOrderBook) {
        level.percentage = (double)level.volume / totalVolume * 100.0;
    }
    
    // Find max volume for highlighting
    m_maxVolume = 0;
    for (const auto &level : m_combinedOrderBook) {
        m_maxVolume = qMax(m_maxVolume, level.volume);
    }
}

void MicroOrderBookModel::updateHighlighting()
{
    for (auto &level : m_combinedOrderBook) {
        // Highlight based on volume threshold and percentage
        bool volumeHighlight = level.volume >= m_volumeThreshold;
        bool percentageHighlight = level.percentage >= m_highlightThreshold;
        level.isHighlighted = volumeHighlight || percentageHighlight;
    }
}

qint64 MicroOrderBookModel::calculateCumulativeVolume(int index) const
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

QString MicroOrderBookModel::getDepthColor(int index) const
{
    if (index < 0 || index >= m_combinedOrderBook.size()) {
        return "#808080";  // Gray
    }
    
    const MicroOrderBookLevel &level = m_combinedOrderBook[index];
    
    if (level.side == "bid") {
        // Green gradient for bids
        double intensity = (double)level.volume / m_maxVolume;
        int green = 128 + (int)(127 * intensity);
        return QString("#00%1%2").arg(green, 2, 16, QChar('0')).arg("00");
    } else if (level.side == "ask") {
        // Red gradient for asks
        double intensity = (double)level.volume / m_maxVolume;
        int red = 128 + (int)(127 * intensity);
        return QString("#%1%2%3").arg(red, 2, 16, QChar('0')).arg("00").arg("00");
    }
    
    return "#808080";  // Gray
} 

}