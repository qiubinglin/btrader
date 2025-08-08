#include "ticktrademodel.h"

namespace btra::gui {

TickTradeModel::TickTradeModel(const QString &name, TickTradeBookSPtr book, QObject *parent)
    : QAbstractListModel(parent), name_(name), ticktradebook_(book) {
    if (ticktradebook_) {
        ticktradebook_->add_notice([this](NotifyType type) { 
            beginResetModel();
            endResetModel();
            emit this->dataChanged(); 
        });
    }
}

int TickTradeModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid()) return 0;
    if (!ticktradebook_) return 0;
    return std::min(max_count_, ticktradebook_->count());
}

QVariant TickTradeModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || !ticktradebook_ || index.row() >= ticktradebook_->count()) return QVariant();

    const TickTradeData &data = ticktradebook_->at(index.row());

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

QHash<int, QByteArray> TickTradeModel::roleNames() const {
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

void TickTradeModel::setMaxCount(int count) {
    if (max_count_ != count) {
        max_count_ = count;
        beginResetModel();
        endResetModel();
        emit dataChanged();
    }
}

int TickTradeModel::getMaxCount() const { return max_count_; }

int TickTradeModel::getCount() const { 
    if (!ticktradebook_) return 0;
    return std::min(max_count_, ticktradebook_->count()); 
}

double TickTradeModel::getLatestPrice() const { 
    if (!ticktradebook_) return 0.0;
    return ticktradebook_->get_latest_price(); 
}

qint64 TickTradeModel::getTotalVolume() const { 
    if (!ticktradebook_) return 0;
    return ticktradebook_->get_total_volume(); 
}

} // namespace btra::gui