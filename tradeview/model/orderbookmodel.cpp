#include "orderbookmodel.h"

#include <QDebug>
#include <algorithm>
#include <QtMath>

namespace btra::gui {

OrderBookModel::OrderBookModel(const QString &name, OrderBookSetSPtr orderbookset, QObject *parent)
    : QAbstractListModel(parent), name_(name), orderbookset_(orderbookset), m_maxLevels(20) {
    orderbookset_->add_notice([this](NotifyType type) { 
        updateCombinedOrderBook();
        beginResetModel();
        endResetModel();
        emit this->dataChanged();
        emit orderBookUpdated();
    });
    
    // Initialize with existing data if available
    if (orderbookset_->size() > 0) {
        updateCombinedOrderBook();
        beginResetModel();
        endResetModel();
    }
}

int OrderBookModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid()) return 0;
    return m_combinedOrderBook.size();
}

QVariant OrderBookModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= m_combinedOrderBook.size()) return QVariant();

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

QHash<int, QByteArray> OrderBookModel::roleNames() const {
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

void OrderBookModel::setMaxLevels(int levels) { 
    if (m_maxLevels != levels) {
        m_maxLevels = levels;
        updateCombinedOrderBook();
        beginResetModel();
        endResetModel();
        emit dataChanged();
    }
}

int OrderBookModel::getMaxLevels() const { return m_maxLevels; }

double OrderBookModel::getBestBid() const { 
    if (orderbookset_ && !orderbookset_->empty()) {
        return orderbookset_->last_orderbook().get_best_bid(); 
    }
    return 0.0;
}

double OrderBookModel::getBestAsk() const { 
    if (orderbookset_ && !orderbookset_->empty()) {
        return orderbookset_->last_orderbook().get_best_ask(); 
    }
    return 0.0;
}

double OrderBookModel::getSpread() const {
    auto best_bid = getBestBid();
    auto best_ask = getBestAsk();
    if (best_bid > 0 && best_ask > 0) {
        return best_ask - best_bid;
    }
    return 0;
}

double OrderBookModel::getMidPrice() const {
    auto best_bid = getBestBid();
    auto best_ask = getBestAsk();
    if (best_bid > 0 && best_ask > 0) {
        return (best_bid + best_ask) / 2.0;
    }
    return 0;
}

QVariantList OrderBookModel::getBids() const {
    QVariantList result;
    if (orderbookset_ && !orderbookset_->empty()) {
        for (const auto &bid : orderbookset_->last_orderbook().get_bids()) {
            QVariantMap level;
            level["price"] = bid.price;
            level["volume"] = bid.volume;
            level["orderCount"] = bid.orderCount;
            level["side"] = bid.side;
            result.append(level);
        }
    }
    return result;
}

QVariantList OrderBookModel::getAsks() const {
    QVariantList result;
    if (orderbookset_ && !orderbookset_->empty()) {
        for (const auto &ask : orderbookset_->last_orderbook().get_asks()) {
            QVariantMap level;
            level["price"] = ask.price;
            level["volume"] = ask.volume;
            level["orderCount"] = ask.orderCount;
            level["side"] = ask.side;
            result.append(level);
        }
    }
    return result;
}

QVariantList OrderBookModel::getOrderBook() const {
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

void OrderBookModel::updateCombinedOrderBook() {
    m_combinedOrderBook.clear();

    if (!orderbookset_ || orderbookset_->empty()) {
        return;
    }

    // Add asks (in ascending order for display - lowest ask first)
    const auto& asks = orderbookset_->last_orderbook().get_asks();
    int askCount = m_maxLevels > 0 ? qMin(asks.size(), m_maxLevels) : asks.size();
    for (int i = 0; i < askCount; ++i) {
        m_combinedOrderBook.append(asks[i]);
    }

    // Add bids (in descending order for display - highest bid first)
    const auto& bids = orderbookset_->last_orderbook().get_bids();
    int bidCount = m_maxLevels > 0 ? qMin(bids.size(), m_maxLevels) : bids.size();
    for (int i = 0; i < bidCount; ++i) {
        m_combinedOrderBook.append(bids[i]);
    }
}

qint64 OrderBookModel::calculateCumulativeVolume(int index) const {
    if (index < 0 || index >= m_combinedOrderBook.size()) {
        return 0;
    }

    qint64 cumulative = 0;
    const QString &side = m_combinedOrderBook[index].side;

    if (side == "ask") {
        // For asks, sum from best ask (first ask) to current level
        for (int i = 0; i <= index && m_combinedOrderBook[i].side == "ask"; ++i) {
            cumulative += m_combinedOrderBook[i].volume;
        }
    } else if (side == "bid") {
        // For bids, sum from best bid (first bid after asks) to current level
        int bidStartIndex = -1;
        for (int i = 0; i < m_combinedOrderBook.size(); ++i) {
            if (m_combinedOrderBook[i].side == "bid") {
                bidStartIndex = i;
                break;
            }
        }
        if (bidStartIndex >= 0) {
            for (int i = bidStartIndex; i <= index && m_combinedOrderBook[i].side == "bid"; ++i) {
                cumulative += m_combinedOrderBook[i].volume;
            }
        }
    }

    return cumulative;
}

double OrderBookModel::calculateDepthPercentage(int index) const {
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

    // Calculate cumulative volume for this level
    qint64 cumulativeVolume = calculateCumulativeVolume(index);

    // Return cumulative depth percentage
    return (double)cumulativeVolume / totalVolume * 100.0;
}

} // namespace btra::gui