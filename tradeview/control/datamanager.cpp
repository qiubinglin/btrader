#include "datamanager.h"
#include "../model/candlestickmodel.h"
#include "../model/ticktrademodel.h"
#include "../model/orderbookmodel.h"
#include "../model/footprintmodel.h"
#include "../model/microorderbookmodel.h"

#include <QTimer>
#include <QDateTime>
#include <QDebug>
#include <QRandomGenerator>

DataManager::DataManager(QObject *parent)
    : QObject(parent)
    , m_candlestickModel(nullptr)
    , m_tickTradeModel(nullptr)
    , m_orderBookModel(nullptr)
    , m_footprintModel(nullptr)
    , m_microOrderBookModel(nullptr)
    , m_isConnected(false)
    , m_simulationTimer(nullptr)
    , m_connectionTimer(nullptr)
{
}

DataManager::~DataManager()
{
    if (m_simulationTimer) {
        m_simulationTimer->stop();
        delete m_simulationTimer;
    }
    if (m_connectionTimer) {
        m_connectionTimer->stop();
        delete m_connectionTimer;
    }
}

bool DataManager::initialize()
{
    qDebug() << "Initializing DataManager...";
    
    // 创建定时器
    m_simulationTimer = new QTimer(this);
    m_connectionTimer = new QTimer(this);
    
    // 连接信号
    connect(m_simulationTimer, &QTimer::timeout, this, &DataManager::onSimulateDataUpdate);
    connect(m_connectionTimer, &QTimer::timeout, this, &DataManager::onConnectionTimeout);
    
    // 初始化模拟数据
    initializeSimulatedData();
    
    qDebug() << "DataManager initialized successfully";
    return true;
}

void DataManager::setModels(CandlestickModel* candlestickModel,
                           TickTradeModel* tickTradeModel,
                           OrderBookModel* orderBookModel,
                           FootprintModel* footprintModel,
                           MicroOrderBookModel* microOrderBookModel)
{
    m_candlestickModel = candlestickModel;
    m_tickTradeModel = tickTradeModel;
    m_orderBookModel = orderBookModel;
    m_footprintModel = footprintModel;
    m_microOrderBookModel = microOrderBookModel;
}

bool DataManager::connectDataSource(const QString& dataSource)
{
    qDebug() << "Connecting to data source:" << dataSource;
    
    m_dataSource = dataSource;
    
    // 模拟连接过程
    m_connectionTimer->setSingleShot(true);
    m_connectionTimer->start(1000); // 1秒后连接成功
    
    return true;
}

void DataManager::disconnectDataSource()
{
    qDebug() << "Disconnecting from data source";
    
    updateConnectionStatus(false);
    m_simulationTimer->stop();
    m_connectionTimer->stop();
}

bool DataManager::subscribeSymbol(const QString& symbol)
{
    qDebug() << "Subscribing to symbol:" << symbol;
    
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

void DataManager::unsubscribeSymbol(const QString& symbol)
{
    qDebug() << "Unsubscribing from symbol:" << symbol;
    
    m_subscribedSymbols.removeOne(symbol);
    m_lastPrices.remove(symbol);
    m_lastUpdateTime.remove(symbol);
    
    // 如果没有订阅的交易对，停止定时器
    if (m_subscribedSymbols.isEmpty()) {
        m_simulationTimer->stop();
    }
}

void DataManager::onSimulateDataUpdate()
{
    for (const QString& symbol : m_subscribedSymbols) {
        // 生成模拟数据
        generateSimulatedCandlestickData(symbol);
        generateSimulatedTickTradeData(symbol);
        generateSimulatedOrderBookData(symbol);
        generateSimulatedFootprintData(symbol);
        generateSimulatedMicroOrderBookData(symbol);
        
        // 更新最后更新时间
        m_lastUpdateTime[symbol] = QDateTime::currentDateTime();
        
        // 发送数据更新信号
        emit dataUpdated(symbol, "all");
    }
}

void DataManager::onConnectionTimeout()
{
    qDebug() << "Connection timeout, simulating successful connection";
    updateConnectionStatus(true);
    
    // 开始模拟数据更新
    if (!m_subscribedSymbols.isEmpty()) {
        m_simulationTimer->start(1000);
    }
}

void DataManager::initializeSimulatedData()
{
    // 初始化默认交易对
    QStringList defaultSymbols = {"BTC/USDT", "ETH/USDT", "BNB/USDT"};
    for (const QString& symbol : defaultSymbols) {
        m_lastPrices[symbol] = 50000.0 + QRandomGenerator::global()->bounded(1000.0);
        m_lastUpdateTime[symbol] = QDateTime::currentDateTime();
    }
}

void DataManager::generateSimulatedCandlestickData(const QString& symbol)
{
    if (!m_candlestickModel) return;
    
    // 生成模拟K线数据
    double basePrice = m_lastPrices[symbol];
    double change = (QRandomGenerator::global()->bounded(200) - 100) / 100.0; // -1% 到 +1%
    double newPrice = basePrice * (1 + change);
    
    // 更新最新价格
    m_lastPrices[symbol] = newPrice;
    
    // 创建K线数据
    QDateTime timestamp = QDateTime::currentDateTime();
    double open = basePrice;
    double close = newPrice;
    double high = qMax(open, close) + QRandomGenerator::global()->bounded(50.0);
    double low = qMin(open, close) - QRandomGenerator::global()->bounded(50.0);
    qint64 volume = QRandomGenerator::global()->bounded(1000) + 100;
    double amount = volume * close;
    
    CandlestickData candlestick(timestamp, open, high, low, close, volume, amount);
    m_candlestickModel->add_candlestick(candlestick);
    
    qDebug() << "Generated candlestick for" << symbol << ":" << open << high << low << close << volume;
}

void DataManager::generateSimulatedTickTradeData(const QString& symbol)
{
    if (!m_tickTradeModel) return;
    
    // 生成模拟逐笔成交数据
    double price = m_lastPrices[symbol] + (QRandomGenerator::global()->bounded(200) - 100) / 100.0;
    qint64 volume = QRandomGenerator::global()->bounded(100) + 1;
    bool isBuy = QRandomGenerator::global()->bounded(2) == 1;
    
    // 更新最新价格
    m_lastPrices[symbol] = price;
    
    // 创建逐笔成交数据
    QDateTime timestamp = QDateTime::currentDateTime();
    QString direction = isBuy ? "buy" : "sell";
    QString tradeId = QString("T%1").arg(QDateTime::currentMSecsSinceEpoch());
    
    TickTradeData tickData(timestamp, price, volume, direction, tradeId, symbol);
    m_tickTradeModel->addTickTrade(tickData);
    
    qDebug() << "Generated tick trade for" << symbol << ":" << price << volume << (isBuy ? "BUY" : "SELL");
}

void DataManager::generateSimulatedOrderBookData(const QString& symbol)
{
    if (!m_orderBookModel) return;
    
    // 生成模拟买卖档位数据
    double basePrice = m_lastPrices[symbol];
    
    // 清空现有数据
    m_orderBookModel->clear();
    
    // 生成买单数据
    QVector<OrderBookLevel> bids;
    for (int i = 0; i < 10; ++i) {
        double price = basePrice - (i + 1) * 0.1;
        qint64 volume = QRandomGenerator::global()->bounded(100) + 10;
        int orderCount = QRandomGenerator::global()->bounded(20) + 1;
        OrderBookLevel level(price, volume, orderCount, "bid");
        bids.append(level);
    }
    
    // 生成卖单数据
    QVector<OrderBookLevel> asks;
    for (int i = 0; i < 10; ++i) {
        double price = basePrice + (i + 1) * 0.1;
        qint64 volume = QRandomGenerator::global()->bounded(100) + 10;
        int orderCount = QRandomGenerator::global()->bounded(20) + 1;
        OrderBookLevel level(price, volume, orderCount, "ask");
        asks.append(level);
    }
    
    m_orderBookModel->updateOrderBook(bids, asks);
    
    qDebug() << "Generated order book for" << symbol << "with" << bids.size() << "bids and" << asks.size() << "asks";
}

void DataManager::generateSimulatedFootprintData(const QString& symbol)
{
    if (!m_footprintModel) return;
    
    // 生成模拟足迹图数据
    Q_UNUSED(symbol) // 暂时未使用，避免编译警告
    
    // 这里可以调用模型的方法来添加数据
    // m_footprintModel->addTickData(...);
}

void DataManager::generateSimulatedMicroOrderBookData(const QString& symbol)
{
    if (!m_microOrderBookModel) return;
    
    // 生成模拟微盘口数据
    Q_UNUSED(symbol) // 暂时未使用，避免编译警告
    
    // 这里可以调用模型的方法来更新数据
    // m_microOrderBookModel->updateMicroOrderBook(...);
}

void DataManager::updateConnectionStatus(bool connected)
{
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