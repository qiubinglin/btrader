#ifndef TICKTRADEMODEL_H
#define TICKTRADEMODEL_H

#include <QAbstractListModel>
#include <QDateTime>
#include <QObject>
#include <QVector>

#include "guidb/ticktradebook.h"

namespace btra::gui {

class TickTradeModel : public QAbstractListModel {
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

    explicit TickTradeModel(const QString& name, TickTradeBookSPtr book, QObject* parent = nullptr);

    // QAbstractListModel interface
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    // Public methods
    Q_INVOKABLE void setMaxCount(int count);
    Q_INVOKABLE int getMaxCount() const;
    Q_INVOKABLE int getCount() const;
    Q_INVOKABLE double getLatestPrice() const;
    Q_INVOKABLE qint64 getTotalVolume() const;

signals:
    void dataChanged();
    void tickTradeAdded();
    void latestPriceChanged(double price);

private:
    QString name_;
    TickTradeBookSPtr ticktradebook_{nullptr};
    int max_count_{1000};
};

} // namespace btra::gui

#endif // TICKTRADEMODEL_H