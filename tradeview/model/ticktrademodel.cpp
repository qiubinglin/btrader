#include "ticktrademodel.h"
#include <QDebug>

namespace btra::gui {

TickTradeModel::TickTradeModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_maxCount(1000)
    , m_latestPrice(0)
    , m_totalVolume(0)
{
}

int TickTradeModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_tickTrades.size();
}

QVariant TickTradeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_tickTrades.size())
        return QVariant();

    const TickTradeData &data = m_tickTrades[index.row()];

    switch (role) {
    case TimestampRole:
        return data.timestamp;
    case PriceRole:
        return data.price;
    case VolumeRole:
        return data.volume;
    case DirectionRole:
        return data.direction;
    case TradeIdRole:
        return data.tradeId;
    case SymbolRole:
        return data.symbol;
    case IsBuyRole:
        return data.direction == "buy";
    case IsSellRole:
        return data.direction == "sell";
    case AmountRole:
        return data.price * data.volume;
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> TickTradeModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[TimestampRole] = "timestamp";
    roles[PriceRole] = "price";
    roles[VolumeRole] = "volume";
    roles[DirectionRole] = "direction";
    roles[TradeIdRole] = "tradeId";
    roles[SymbolRole] = "symbol";
    roles[IsBuyRole] = "isBuy";
    roles[IsSellRole] = "isSell";
    roles[AmountRole] = "amount";
    return roles;
}

void TickTradeModel::addTickTrade(const TickTradeData &data)
{
    beginInsertRows(QModelIndex(), 0, 0);
    m_tickTrades.prepend(data);
    endInsertRows();

    // Remove old data if exceeds max count
    if (m_tickTrades.size() > m_maxCount) {
        beginRemoveRows(QModelIndex(), m_maxCount, m_tickTrades.size() - 1);
        m_tickTrades.remove(m_maxCount, m_tickTrades.size() - m_maxCount);
        endRemoveRows();
    }

    updateLatestPrice();
    updateTotalVolume();

    emit dataChanged();
    emit tickTradeAdded();
}

void TickTradeModel::clear()
{
    if (m_tickTrades.isEmpty())
        return;

    beginResetModel();
    m_tickTrades.clear();
    endResetModel();

    m_latestPrice = 0;
    m_totalVolume = 0;

    emit dataChanged();
}

void TickTradeModel::setMaxCount(int count)
{
    if (m_maxCount != count) {
        m_maxCount = count;
        
        // Remove excess data
        if (m_tickTrades.size() > m_maxCount) {
            beginRemoveRows(QModelIndex(), m_maxCount, m_tickTrades.size() - 1);
            m_tickTrades.remove(m_maxCount, m_tickTrades.size() - m_maxCount);
            endRemoveRows();
        }
        
        emit dataChanged();
    }
}

int TickTradeModel::getMaxCount() const
{
    return m_maxCount;
}

int TickTradeModel::getCount() const
{
    return m_tickTrades.size();
}

TickTradeData TickTradeModel::getTickTrade(int index) const
{
    if (index >= 0 && index < m_tickTrades.size()) {
        return m_tickTrades[index];
    }
    return TickTradeData();
}

QVariantList TickTradeModel::getTickTrades(int start, int count) const
{
    QVariantList result;
    int end = qMin(start + count, m_tickTrades.size());
    
    for (int i = start; i < end; ++i) {
        QVariantMap tickTrade;
        const TickTradeData &data = m_tickTrades[i];
        tickTrade["timestamp"] = data.timestamp;
        tickTrade["price"] = data.price;
        tickTrade["volume"] = data.volume;
        tickTrade["direction"] = data.direction;
        tickTrade["tradeId"] = data.tradeId;
        tickTrade["symbol"] = data.symbol;
        result.append(tickTrade);
    }
    
    return result;
}

double TickTradeModel::getLatestPrice() const
{
    return m_latestPrice;
}

qint64 TickTradeModel::getTotalVolume() const
{
    return m_totalVolume;
}

void TickTradeModel::updateLatestPrice()
{
    if (!m_tickTrades.isEmpty()) {
        double newPrice = m_tickTrades.first().price;
        if (m_latestPrice != newPrice) {
            m_latestPrice = newPrice;
            emit latestPriceChanged(m_latestPrice);
        }
    }
}

void TickTradeModel::updateTotalVolume()
{
    m_totalVolume = 0;
    for (const auto &data : m_tickTrades) {
        m_totalVolume += data.volume;
    }
} 

}