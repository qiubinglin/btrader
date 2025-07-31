#include <napi.h>
#include "engine.h"
#include <memory>

std::unique_ptr<TradingEngine> engine;

Napi::Value CreateEngine(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    engine = std::make_unique<TradingEngine>();
    return Napi::Boolean::New(env, true);
}

Napi::Value AddOrder(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (!engine) {
        Napi::TypeError::New(env, "Engine not initialized").ThrowAsJavaScriptException();
        return env.Null();
    }
    
    if (info.Length() < 1 || !info[0].IsObject()) {
        Napi::TypeError::New(env, "Expected object").ThrowAsJavaScriptException();
        return env.Null();
    }
    
    Napi::Object orderObj = info[0].As<Napi::Object>();
    
    Order order;
    order.symbol = orderObj.Get("symbol").As<Napi::String>().Utf8Value();
    order.side = orderObj.Get("side").As<Napi::String>().Utf8Value();
    order.type = orderObj.Get("type").As<Napi::String>().Utf8Value();
    order.price = orderObj.Get("price").As<Napi::Number>().DoubleValue();
    order.quantity = orderObj.Get("quantity").As<Napi::Number>().Int32Value();
    
    uint64_t orderId = engine->addOrder(order);
    return Napi::Number::New(env, orderId);
}

Napi::Value GetOrderBook(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (!engine) {
        Napi::TypeError::New(env, "Engine not initialized").ThrowAsJavaScriptException();
        return env.Null();
    }
    
    std::string symbol = info[0].As<Napi::String>().Utf8Value();
    OrderBook book = engine->getOrderBook(symbol);
    
    Napi::Object result = Napi::Object::New(env);
    result.Set("symbol", Napi::String::New(env, book.symbol));
    result.Set("timestamp", Napi::Number::New(env, book.timestamp));
    
    Napi::Array bids = Napi::Array::New(env, book.bids.size());
    for (size_t i = 0; i < book.bids.size(); ++i) {
        Napi::Object bid = Napi::Object::New(env);
        bid.Set("price", Napi::Number::New(env, book.bids[i].price));
        bid.Set("quantity", Napi::Number::New(env, book.bids[i].quantity));
        bid.Set("orders", Napi::Number::New(env, book.bids[i].orders));
        bids.Set(i, bid);
    }
    
    Napi::Array asks = Napi::Array::New(env, book.asks.size());
    for (size_t i = 0; i < book.asks.size(); ++i) {
        Napi::Object ask = Napi::Object::New(env);
        ask.Set("price", Napi::Number::New(env, book.asks[i].price));
        ask.Set("quantity", Napi::Number::New(env, book.asks[i].quantity));
        ask.Set("orders", Napi::Number::New(env, book.asks[i].orders));
        asks.Set(i, ask);
    }
    
    result.Set("bids", bids);
    result.Set("asks", asks);
    
    return result;
}

Napi::Value GetTrades(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (!engine) {
        return Napi::Array::New(env, 0);
    }
    
    std::string symbol = info[0].As<Napi::String>().Utf8Value();
    int limit = info.Length() > 1 ? info[1].As<Napi::Number>().Int32Value() : 100;
    
    std::vector<Trade> trades = engine->getTrades(symbol, limit);
    Napi::Array result = Napi::Array::New(env, trades.size());
    
    for (size_t i = 0; i < trades.size(); ++i) {
        Napi::Object trade = Napi::Object::New(env);
        trade.Set("id", Napi::Number::New(env, trades[i].id));
        trade.Set("symbol", Napi::String::New(env, trades[i].symbol));
        trade.Set("price", Napi::Number::New(env, trades[i].price));
        trade.Set("quantity", Napi::Number::New(env, trades[i].quantity));
        trade.Set("side", Napi::String::New(env, trades[i].side));
        trade.Set("timestamp", Napi::Number::New(env, trades[i].timestamp));
        result.Set(i, trade);
    }
    
    return result;
}

Napi::Value StartSimulation(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (engine) {
        engine->startMarketDataSimulation();
    }
    
    return Napi::Boolean::New(env, true);
}

Napi::Value StopSimulation(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (engine) {
        engine->stopMarketDataSimulation();
    }
    
    return Napi::Boolean::New(env, true);
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set("createEngine", Napi::Function::New(env, CreateEngine));
    exports.Set("addOrder", Napi::Function::New(env, AddOrder));
    exports.Set("getOrderBook", Napi::Function::New(env, GetOrderBook));
    exports.Set("getTrades", Napi::Function::New(env, getTrades));
    exports.Set("startSimulation", Napi::Function::New(env, StartSimulation));
    exports.Set("stopSimulation", Napi::Function::New(env, StopSimulation));
    
    return exports;
}

NODE_API_MODULE(trading_engine, Init)