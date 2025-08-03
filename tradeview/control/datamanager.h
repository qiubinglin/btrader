#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QObject>
#include <QTimer>
#include <QDateTime>
#include <QMap>
#include <QString>

// 前向声明
class CandlestickModel;
class TickTradeModel;
class OrderBookModel;
class FootprintModel;
class MicroOrderBookModel;

/**
 * @brief 数据管理器类
 * 
 * 负责管理所有交易数据的获取、处理和分发，包括：
 * - 实时数据订阅
 * - 数据预处理和转换
 * - 数据分发到各个模型
 * - 数据缓存管理
 */
class DataManager : public QObject
{
    Q_OBJECT

public:
    explicit DataManager(QObject *parent = nullptr);
    ~DataManager();

    /**
     * @brief 初始化数据管理器
     * @return 初始化是否成功
     */
    bool initialize();

    /**
     * @brief 设置数据模型
     * @param candlestickModel K线数据模型
     * @param tickTradeModel 逐笔成交模型
     * @param orderBookModel 买卖档位模型
     * @param footprintModel 足迹图模型
     * @param microOrderBookModel 微盘口模型
     */
    void setModels(CandlestickModel* candlestickModel,
                   TickTradeModel* tickTradeModel,
                   OrderBookModel* orderBookModel,
                   FootprintModel* footprintModel,
                   MicroOrderBookModel* microOrderBookModel);

    /**
     * @brief 连接数据源
     * @param dataSource 数据源地址
     * @return 连接是否成功
     */
    Q_INVOKABLE bool connectDataSource(const QString& dataSource);

    /**
     * @brief 断开数据源连接
     */
    Q_INVOKABLE void disconnectDataSource();

    /**
     * @brief 订阅交易对数据
     * @param symbol 交易对符号
     * @return 订阅是否成功
     */
    Q_INVOKABLE bool subscribeSymbol(const QString& symbol);

    /**
     * @brief 取消订阅交易对数据
     * @param symbol 交易对符号
     */
    Q_INVOKABLE void unsubscribeSymbol(const QString& symbol);

    /**
     * @brief 获取当前连接状态
     * @return 连接状态
     */
    Q_INVOKABLE bool isConnected() const { return m_isConnected; }

    /**
     * @brief 获取当前订阅的交易对列表
     * @return 交易对列表
     */
    Q_INVOKABLE QStringList getSubscribedSymbols() const { return m_subscribedSymbols; }

signals:
    /**
     * @brief 连接状态改变信号
     * @param connected 是否已连接
     */
    void connectionStatusChanged(bool connected);

    /**
     * @brief 数据更新信号
     * @param symbol 交易对符号
     * @param dataType 数据类型
     */
    void dataUpdated(const QString& symbol, const QString& dataType);

    /**
     * @brief 错误信号
     * @param error 错误信息
     */
    void error(const QString& error);

private slots:
    /**
     * @brief 处理模拟数据更新
     */
    void onSimulateDataUpdate();

    /**
     * @brief 处理连接超时
     */
    void onConnectionTimeout();

private:
    /**
     * @brief 初始化模拟数据
     */
    void initializeSimulatedData();

    /**
     * @brief 生成模拟K线数据
     * @param symbol 交易对符号
     */
    void generateSimulatedCandlestickData(const QString& symbol);

    /**
     * @brief 生成模拟逐笔成交数据
     * @param symbol 交易对符号
     */
    void generateSimulatedTickTradeData(const QString& symbol);

    /**
     * @brief 生成模拟买卖档位数据
     * @param symbol 交易对符号
     */
    void generateSimulatedOrderBookData(const QString& symbol);

    /**
     * @brief 生成模拟足迹图数据
     * @param symbol 交易对符号
     */
    void generateSimulatedFootprintData(const QString& symbol);

    /**
     * @brief 生成模拟微盘口数据
     * @param symbol 交易对符号
     */
    void generateSimulatedMicroOrderBookData(const QString& symbol);

    /**
     * @brief 更新连接状态
     * @param connected 是否已连接
     */
    void updateConnectionStatus(bool connected);

private:
    // 数据模型
    CandlestickModel* m_candlestickModel;      ///< K线数据模型
    TickTradeModel* m_tickTradeModel;          ///< 逐笔成交模型
    OrderBookModel* m_orderBookModel;          ///< 买卖档位模型
    FootprintModel* m_footprintModel;          ///< 足迹图模型
    MicroOrderBookModel* m_microOrderBookModel; ///< 微盘口模型

    // 连接状态
    bool m_isConnected;                        ///< 是否已连接
    QString m_dataSource;                      ///< 数据源地址
    QStringList m_subscribedSymbols;           ///< 已订阅的交易对列表

    // 定时器
    QTimer* m_simulationTimer;                 ///< 模拟数据更新定时器
    QTimer* m_connectionTimer;                 ///< 连接超时定时器

    // 模拟数据
    QMap<QString, double> m_lastPrices;        ///< 最新价格缓存
    QMap<QString, QDateTime> m_lastUpdateTime; ///< 最后更新时间
};

#endif // DATAMANAGER_H 