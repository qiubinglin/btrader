#include "datamanager.h"

#include <qdatetime.h>
#include <qlogging.h>

#include <QDateTime>
#include <QRandomGenerator>
#include <QTimer>
#include <utility>

#include "core/types.h"
#include "guidb/guitime.h"
#include "guidb/kline.h"
#include "guidb/orderbookset.h"
#include "guidb/resource.h"

#define ON_MEM_FUNC(func) [this](const EventSPtr& event) { this->func(event); }

namespace btra::gui {

DataManager::DataManager(DatabaseSPtr database, ConfigManager* config_mgr, CoreAgent* coreagent, QObject* parent)
    : QObject(parent),
      m_isConnected(false),
      m_simulationTimer(nullptr),
      m_connectionTimer(nullptr),
      database_(database),
      config_mgr_(config_mgr),
      coreagent_(coreagent) {}

DataManager::~DataManager() {
    if (m_simulationTimer) {
        m_simulationTimer->stop();
        delete m_simulationTimer;
    }
    if (m_connectionTimer) {
        m_connectionTimer->stop();
        delete m_connectionTimer;
    }
}

bool DataManager::initialize() {
    qDebug() << "Initializing DataManager...";

    time_unit_ = config_mgr_->get_core_engine_cfg().get_time_unit();

    // 创建定时器
    m_simulationTimer = new QTimer(this);
    m_connectionTimer = new QTimer(this);

    // 连接信号
    connect(m_simulationTimer, &QTimer::timeout, this, &DataManager::onSimulateDataUpdate);
    connect(m_connectionTimer, &QTimer::timeout, this, &DataManager::onConnectionTimeout);

    // 初始化模拟数据
    initializeSimulatedData();

    event_handlers_ = {{MsgTag::Bar, ON_MEM_FUNC(on_incomming_bar)}};

    qDebug() << "DataManager initialized successfully";
    return true;
}

bool DataManager::connectDataSource(const QString& dataSource) {
    // Connecting to data source: " << dataSource;

    m_dataSource = dataSource;

    // 模拟连接过程
    m_connectionTimer->setSingleShot(true);
    m_connectionTimer->start(1000); // 1秒后连接成功

    return true;
}

void DataManager::disconnectDataSource() {
    // Disconnecting from data source

    updateConnectionStatus(false);
    m_simulationTimer->stop();
    m_connectionTimer->stop();
}

bool DataManager::subscribeSymbol(const QString& symbol) {
    // Subscribing to symbol: " << symbol;

    if (!m_subscribedSymbols.contains(symbol)) {
        m_subscribedSymbols.append(symbol);

        // 初始化该交易对的数据
        m_lastPrices[symbol] = 50000.0 + QRandomGenerator::global()->bounded(1000.0);
        m_lastUpdateTime[symbol] = QDateTime::currentDateTime();

        // 如果已连接，开始模拟数据更新
        if (m_isConnected && !m_simulationTimer->isActive()) {
            m_simulationTimer->start(1000); // 每秒更新一次
        }
    }

    return true;
}

void DataManager::unsubscribeSymbol(const QString& symbol) {
    // Unsubscribing from symbol: " << symbol;

    m_subscribedSymbols.removeOne(symbol);
    m_lastPrices.remove(symbol);
    m_lastUpdateTime.remove(symbol);

    // 如果没有订阅的交易对，停止定时器
    if (m_subscribedSymbols.isEmpty()) {
        m_simulationTimer->stop();
    }
}

void DataManager::onSimulateDataUpdate() {
    for (const QString& symbol : m_subscribedSymbols) {
        // 生成模拟数据
        generateSimulatedCandlestickData(symbol);
        generateSimulatedTickTradeData(symbol);
        generateSimulatedOrderBookData(symbol);

        // 更新最后更新时间
        m_lastUpdateTime[symbol] = QDateTime::currentDateTime();

        // 发送数据更新信号
        emit dataUpdated(symbol, "all");
    }
}

void DataManager::onConnectionTimeout() {
    // Connection timeout, simulating successful connection
    updateConnectionStatus(true);

    // 开始模拟数据更新
    if (!m_subscribedSymbols.isEmpty()) {
        m_simulationTimer->start(1000);
    }
}

void DataManager::initializeSimulatedData() {
    if (config_mgr_->get_simulation()) {
        // 初始化默认交易对
        QStringList defaultSymbols = config_mgr_->getEnabledInstruments();
        for (const QString& symbol : defaultSymbols) {
            subscribeSymbol(symbol);
            m_lastPrices[symbol] = 50000.0 + QRandomGenerator::global()->bounded(1000.0);
            m_lastUpdateTime[symbol] = QDateTime::currentDateTime();
        }

        connectDataSource("simulation");
    }
}

void DataManager::generateSimulatedCandlestickData(const QString& symbol) {
    // 生成模拟K线数据
    double basePrice = m_lastPrices[symbol];
    if (basePrice <= 0) {
        basePrice = 50000.0; // 重置为默认价格
        m_lastPrices[symbol] = basePrice;
    }

    double change = (QRandomGenerator::global()->bounded(200) - 100) / 100.0; // -1% 到 +1%
    double newPrice = basePrice * (1 + change);

    // 确保新价格不会变成负数
    if (newPrice <= 0) {
        newPrice = basePrice; // 保持原价格
    }

    // 更新最新价格
    m_lastPrices[symbol] = newPrice;

    // 创建按分钟对齐的1m K线：第一根基于当前时刻所在分钟，其后每次严格为上一根的下一分钟
    const QDateTime now = QDateTime::currentDateTime();
    const qint64 nowMs = now.toMSecsSinceEpoch();
    const qint64 alignedMs = (nowMs / 60000) * 60000; // 向下取整到分钟起点
    QDateTime alignedStartOfNow = QDateTime::fromMSecsSinceEpoch(alignedMs, now.timeZone());

    auto dataset = database_->reqKlineDB(symbol.toStdString());

    QDateTime startTime;
    if (m_lastKlineStartTime.contains(symbol)) {
        // 已经有上一根，下一根固定为上一根+60s
        startTime = m_lastKlineStartTime[symbol].addSecs(60);
    } else if (dataset->size() > 0) {
        // 若数据库已有最后一根，则在其基础上生成下一分钟
        startTime = dataset->back().start_time.addSecs(60);
    } else {
        // 第一根：以当前时刻所在分钟为起点
        startTime = alignedStartOfNow;
    }

    QDateTime endTime = startTime.addSecs(60);

    // 连续性：若有上一根，开盘取上一根收盘；否则用basePrice
    double open = basePrice;
    if (dataset->size() > 0) {
        open = dataset->back().close;
    }
    double close = newPrice;
    double high = qMax(open, close) + QRandomGenerator::global()->bounded(50.0);
    double low = qMin(open, close) - QRandomGenerator::global()->bounded(50.0);
    qint64 volume = QRandomGenerator::global()->bounded(1000) + 100;
    double amount = volume * close;

    KLine candlestick(startTime, endTime, open, high, low, close, volume, amount);
    dataset->push_back(candlestick);
    if (dataset->size() > 1000) {
        dataset->pop_front();
    }
    dataset->notify_all(NotifyType::Update);

    m_lastKlineStartTime[symbol] = startTime;

    // Generated candlestick for " << symbol << ": " << open << " " << high << " " << low << " " << close << " " <<
    // volume;
}

void DataManager::generateSimulatedTickTradeData(const QString& symbol) {
    // 生成模拟逐笔成交数据
    double price = m_lastPrices[symbol] + (QRandomGenerator::global()->bounded(200) - 100) / 100.0;

    // 确保价格不会变成负数
    if (price <= 0) {
        price = 50000.0; // 重置为默认价格
    }

    qint64 volume = QRandomGenerator::global()->bounded(100) + 1;
    bool isBuy = QRandomGenerator::global()->bounded(2) == 1;

    // 更新最新价格
    m_lastPrices[symbol] = price;

    // 创建逐笔成交数据
    QDateTime timestamp = QDateTime::currentDateTime();
    QString direction = isBuy ? "buy" : "sell";
    QString tradeId = QString("T%1").arg(QDateTime::currentMSecsSinceEpoch());

    TickTradeData tickData(timestamp, price, volume, direction, tradeId, symbol);

    // 添加到数据库
    auto dataset = database_->reqTickTradeBook(symbol.toStdString());
    dataset->add(tickData);
    dataset->notify_all(NotifyType::Update);

    // Generated tick trade for " << symbol << ": " << price << " " << volume << " " << (isBuy ? "BUY" : "SELL");
}

void DataManager::generateSimulatedOrderBookData(const QString& symbol) {
    // 生成模拟买卖档位数据
    double basePrice = m_lastPrices[symbol];
    if (basePrice <= 0) {
        basePrice = 50000.0; // 默认价格
    }

    // 生成买单数据（价格递减，最高价在最前面）
    QVector<OrderBookLevel> bids;
    for (int i = 0; i < 10; ++i) {
        double price = basePrice - (i + 1) * 10.0; // 价格间隔10
        qint64 volume = QRandomGenerator::global()->bounded(1000) + 100;
        int orderCount = QRandomGenerator::global()->bounded(50) + 1;
        OrderBookLevel level(price, volume, orderCount, "bid");
        bids.append(level);
    }

    // 生成卖单数据（价格递增，最低价在最前面）
    QVector<OrderBookLevel> asks;
    for (int i = 0; i < 10; ++i) {
        double price = basePrice + (i + 1) * 10.0; // 价格间隔10
        qint64 volume = QRandomGenerator::global()->bounded(1000) + 100;
        int orderCount = QRandomGenerator::global()->bounded(50) + 1;
        OrderBookLevel level(price, volume, orderCount, "ask");
        asks.append(level);
    }

    // Generated order book for " << symbol << " with " << bids.size() << " bids and " << asks.size() << " asks";
    // Base price: " << basePrice << " Best bid: " << (bids.isEmpty() ? 0 : bids.first().price) << " Best ask: " <<
    // (asks.isEmpty() ? 0 : asks.first().price);

    auto dataset = database_->reqOrderBookSet(symbol.toStdString());
    OrderBook book;
    book.update(std::move(bids), std::move(asks), QDateTime::currentDateTime());
    dataset->add_orderbook(std::move(book));
    if (dataset->size() > 1000) {
        dataset->pop_front();
    }
    dataset->notify_all(NotifyType::Update);
}

void DataManager::updateConnectionStatus(bool connected) {
    if (m_isConnected != connected) {
        m_isConnected = connected;
        emit connectionStatusChanged(connected);

        if (connected) {
            qDebug() << "Connected to data source";
        } else {
            qDebug() << "Disconnected from data source";
        }
    }
}

void DataManager::on_incomming_message() {
    coreagent_->GetCoreComm().read_msg([this](const EventSPtr& event) {
        if (this->event_handlers_.contains(event->msg_type())) {
            this->event_handlers_[event->msg_type()](event);
        }
    });
}

void DataManager::on_incomming_bar(const EventSPtr& event) {
    const auto& bar = event->data<Bar>();
    auto dataset = database_->reqKlineDB(bar.instrument_id);

    QDateTime start, end;
    switch (time_unit_) {
        case infra::TimeUnit::SEC:
            break;
        case infra::TimeUnit::NANO:
            break;
        case infra::TimeUnit::MILLI:
        default:
            start = GuiTime::fromTimestampMs(bar.start_time);
            end = GuiTime::fromTimestampMs(bar.end_time);
            break;
    }

    /* Add bar */
    if (dataset->size() and dataset->back().contains(start)) {
        dataset->back().close = bar.close;
        dataset->back().high = std::max(dataset->back().high, bar.high);
        dataset->back().low = std::max(dataset->back().low, bar.low);
        dataset->back().volume += bar.volume;
    } else {
        KLine kline(end, bar.open, bar.high, bar.low, bar.close, bar.volume, 0);
        dataset->push_back(kline);
    }

    if (dataset->size() > 1000) {
        dataset->pop_front();
    }
    dataset->notify_all(NotifyType::Update);
}

} // namespace btra::gui