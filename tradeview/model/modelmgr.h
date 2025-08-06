#pragma once

#include <QObject>
#include <QVariantMap>

#include "model/candlestickmodel.h"
#include "model/orderbookmodel.h"
#include "model/ticktrademodel.h"
#include "guidb/database.h"

namespace btra::gui {

class ModelMgr : public QObject {
    Q_OBJECT
public:
    explicit ModelMgr(QObject *parent = nullptr);

    void setDatabase(DatabaseSPtr ptr) { database_ = ptr; }

    Q_INVOKABLE CandlestickModel *reqCandlestickModel(const QVariantMap &id);

    Q_INVOKABLE OrderBookModel *reqOrderBookModel(const QVariantMap &id);

    Q_INVOKABLE TickTradeModel *reqTickTradeModel(const QVariantMap &id);

private:
    DatabaseSPtr database_{nullptr};
};

} // namespace btra::gui