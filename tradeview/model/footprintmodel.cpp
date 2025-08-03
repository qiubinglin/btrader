#include "footprintmodel.h"
#include <QDebug>
#include <QTime>

FootprintModel::FootprintModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_timeframe("1m")
    , m_priceStep(0.01)
{
}

int FootprintModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_footprintBars.size();
}

QVariant FootprintModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_footprintBars.size())
        return QVariant();

    const FootprintBar &bar = m_footprintBars[index.row()];

    switch (role) {
    case TimestampRole:
        return bar.timestamp;
    case OpenRole:
        return bar.open;
    case HighRole:
        return bar.high;
    case LowRole:
        return bar.low;
    case CloseRole:
        return bar.close;
    case IsUpRole:
        return bar.close >= bar.open;
    case IsDownRole:
        return bar.close < bar.open;
    case CellsRole:
        return QVariant::fromValue(bar.cells);
    case TotalBuyVolumeRole:
        return calculateTotalBuyVolume(bar);
    case TotalSellVolumeRole:
        return calculateTotalSellVolume(bar);
    case TotalVolumeRole:
        return calculateTotalVolume(bar);
    case DeltaRole:
        return calculateDelta(bar);
    case DeltaPercentRole:
        return calculateDeltaPercent(bar);
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> FootprintModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[TimestampRole] = "timestamp";
    roles[OpenRole] = "open";
    roles[HighRole] = "high";
    roles[LowRole] = "low";
    roles[CloseRole] = "close";
    roles[IsUpRole] = "isUp";
    roles[IsDownRole] = "isDown";
    roles[CellsRole] = "cells";
    roles[TotalBuyVolumeRole] = "totalBuyVolume";
    roles[TotalSellVolumeRole] = "totalSellVolume";
    roles[TotalVolumeRole] = "totalVolume";
    roles[DeltaRole] = "delta";
    roles[DeltaPercentRole] = "deltaPercent";
    return roles;
}

void FootprintModel::addFootprintBar(const FootprintBar &bar)
{
    beginInsertRows(QModelIndex(), m_footprintBars.size(), m_footprintBars.size());
    m_footprintBars.append(bar);
    endInsertRows();

    emit dataChanged();
    emit footprintBarAdded();
}

void FootprintModel::updateFootprintBar(const FootprintBar &bar)
{
    // Find the bar with the same timestamp
    for (int i = 0; i < m_footprintBars.size(); ++i) {
        if (m_footprintBars[i].timestamp == bar.timestamp) {
            m_footprintBars[i] = bar;
            QModelIndex index = createIndex(i, 0);
            emit QAbstractListModel::dataChanged(index, index);
            emit footprintBarUpdated(i);
            break;
        }
    }
}

void FootprintModel::addTickData(const QDateTime &timestamp, double price, qint64 volume, const QString &direction)
{
    QDateTime barStartTime = getBarStartTime(timestamp);
    updateCurrentBar(barStartTime, price, volume, direction);
}

void FootprintModel::clear()
{
    if (m_footprintBars.isEmpty())
        return;

    beginResetModel();
    m_footprintBars.clear();
    m_currentBarMap.clear();
    endResetModel();

    emit dataChanged();
}

void FootprintModel::setTimeframe(const QString &timeframe)
{
    if (m_timeframe != timeframe) {
        m_timeframe = timeframe;
        emit dataChanged();
    }
}

QString FootprintModel::getTimeframe() const
{
    return m_timeframe;
}

void FootprintModel::setPriceStep(double step)
{
    if (m_priceStep != step) {
        m_priceStep = step;
        emit dataChanged();
    }
}

double FootprintModel::getPriceStep() const
{
    return m_priceStep;
}

int FootprintModel::getCount() const
{
    return m_footprintBars.size();
}

FootprintBar FootprintModel::getFootprintBar(int index) const
{
    if (index >= 0 && index < m_footprintBars.size()) {
        return m_footprintBars[index];
    }
    return FootprintBar();
}

QVariantList FootprintModel::getFootprintBars(int start, int count) const
{
    QVariantList result;
    int end = qMin(start + count, m_footprintBars.size());
    
    for (int i = start; i < end; ++i) {
        QVariantMap bar;
        const FootprintBar &footprintBar = m_footprintBars[i];
        bar["timestamp"] = footprintBar.timestamp;
        bar["open"] = footprintBar.open;
        bar["high"] = footprintBar.high;
        bar["low"] = footprintBar.low;
        bar["close"] = footprintBar.close;
        
        QVariantList cells;
        for (const auto &cell : footprintBar.cells) {
            QVariantMap cellMap;
            cellMap["price"] = cell.price;
            cellMap["buyVolume"] = cell.buyVolume;
            cellMap["sellVolume"] = cell.sellVolume;
            cellMap["totalVolume"] = cell.totalVolume;
            cellMap["buyCount"] = cell.buyCount;
            cellMap["sellCount"] = cell.sellCount;
            cellMap["totalCount"] = cell.totalCount;
            cellMap["delta"] = cell.delta;
            cellMap["deltaPercent"] = cell.deltaPercent;
            cells.append(cellMap);
        }
        bar["cells"] = cells;
        result.append(bar);
    }
    
    return result;
}

void FootprintModel::updateCurrentBar(const QDateTime &timestamp, double price, qint64 volume, const QString &direction)
{
    if (!m_currentBarMap.contains(timestamp)) {
        FootprintBar newBar(timestamp, price, price, price, price);
        m_currentBarMap[timestamp] = newBar;
    }

    FootprintBar &bar = m_currentBarMap[timestamp];
    
    // Update OHLC
    if (price > bar.high) bar.high = price;
    if (price < bar.low) bar.low = price;
    bar.close = price;

    // Round price to step
    double roundedPrice = roundToPriceStep(price);
    
    // Find or create cell for this price
    FootprintCell *cell = nullptr;
    for (auto &existingCell : bar.cells) {
        if (qAbs(existingCell.price - roundedPrice) < 0.0001) {
            cell = &existingCell;
            break;
        }
    }
    
    if (!cell) {
        bar.cells.append(FootprintCell(roundedPrice));
        cell = &bar.cells.last();
    }

    // Update cell data
    if (direction == "buy") {
        cell->buyVolume += volume;
        cell->buyCount++;
    } else if (direction == "sell") {
        cell->sellVolume += volume;
        cell->sellCount++;
    }
    
    cell->totalVolume = cell->buyVolume + cell->sellVolume;
    cell->totalCount = cell->buyCount + cell->sellCount;
    cell->delta = cell->buyVolume - cell->sellVolume;
    cell->deltaPercent = cell->totalVolume > 0 ? (cell->delta / cell->totalVolume) * 100.0 : 0;

    calculateBarStatistics(bar);
}

QDateTime FootprintModel::getBarStartTime(const QDateTime &timestamp) const
{
    QDateTime barStart = timestamp;
    
    if (m_timeframe == "1m") {
        barStart.setTime(QTime(barStart.time().hour(), barStart.time().minute(), 0, 0));
    } else if (m_timeframe == "5m") {
        int minute = barStart.time().minute();
        minute = (minute / 5) * 5;
        barStart.setTime(QTime(barStart.time().hour(), minute, 0, 0));
    } else if (m_timeframe == "15m") {
        int minute = barStart.time().minute();
        minute = (minute / 15) * 15;
        barStart.setTime(QTime(barStart.time().hour(), minute, 0, 0));
    } else if (m_timeframe == "1h") {
        barStart.setTime(QTime(barStart.time().hour(), 0, 0, 0));
    }
    
    return barStart;
}

double FootprintModel::roundToPriceStep(double price) const
{
    return qRound(price / m_priceStep) * m_priceStep;
}

void FootprintModel::calculateBarStatistics(FootprintBar &bar)
{
    // Sort cells by price
    std::sort(bar.cells.begin(), bar.cells.end(), [](const FootprintCell &a, const FootprintCell &b) {
        return a.price < b.price;
    });
}

qint64 FootprintModel::calculateTotalBuyVolume(const FootprintBar &bar) const
{
    qint64 total = 0;
    for (const auto &cell : bar.cells) {
        total += cell.buyVolume;
    }
    return total;
}

qint64 FootprintModel::calculateTotalSellVolume(const FootprintBar &bar) const
{
    qint64 total = 0;
    for (const auto &cell : bar.cells) {
        total += cell.sellVolume;
    }
    return total;
}

qint64 FootprintModel::calculateTotalVolume(const FootprintBar &bar) const
{
    return calculateTotalBuyVolume(bar) + calculateTotalSellVolume(bar);
}

double FootprintModel::calculateDelta(const FootprintBar &bar) const
{
    return calculateTotalBuyVolume(bar) - calculateTotalSellVolume(bar);
}

double FootprintModel::calculateDeltaPercent(const FootprintBar &bar) const
{
    qint64 totalVolume = calculateTotalVolume(bar);
    if (totalVolume == 0) return 0;
    return (calculateDelta(bar) / totalVolume) * 100.0;
} 