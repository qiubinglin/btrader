#include "modelmgr.h"

#include <map>
#include "orderbookmodel.h"

namespace btra::gui {

static std::map<QString, CandlestickModel *> s_candlestick_models;

static std::map<QString, OrderBookModel *> s_orderbook_models;

static std::map<QString, TickTradeModel *> s_ticktrade_models;

ModelMgr::ModelMgr(QObject *parent) : QObject(parent) {}

CandlestickModel *ModelMgr::reqCandlestickModel(const QVariantMap &id) {
    QString symbol = id["symbol"].toString();
    if (symbol.isEmpty()) {
        return nullptr;
    }
    if (!s_candlestick_models.contains(symbol)) {
        s_candlestick_models[symbol] = new CandlestickModel(symbol, database_->reqKlineDB(symbol.toStdString()), this);
    }
    return s_candlestick_models[symbol];
}

OrderBookModel *ModelMgr::reqOrderBookModel(const QVariantMap &id) {
    QString symbol = id["symbol"].toString();
    if (symbol.isEmpty()) {
        return nullptr;
    }
    if (!s_orderbook_models.contains(symbol)) {
        s_orderbook_models[symbol] = new OrderBookModel(symbol, database_->reqOrderBookSet(symbol.toStdString()), this);
    }
    return s_orderbook_models[symbol];
}

TickTradeModel *ModelMgr::reqTickTradeModel(const QVariantMap &id) {
    QString symbol = id["symbol"].toString();
    if (symbol.isEmpty()) {
        return nullptr;
    }
    if (!s_ticktrade_models.contains(symbol)) {
        s_ticktrade_models[symbol] = new TickTradeModel(symbol, database_->reqTickTradeBook(symbol.toStdString()), this);
    }
    return s_ticktrade_models[symbol];
}

} // namespace btra::gui