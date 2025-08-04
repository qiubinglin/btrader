#ifndef APPLICATION_H
#define APPLICATION_H

#include <QGuiApplication>
#include <QObject>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>

// 包含模型头文件
#include "candlestickmodel.h"
#include "coreagent.h"
#include "footprintmodel.h"
#include "microorderbookmodel.h"
#include "orderbookmodel.h"
#include "ticktrademodel.h"

namespace btra::gui {

// 前向声明
class DataManager;
class UIManager;

/**
 * @brief 订单流交易系统主应用类
 *
 * 负责管理整个应用程序的生命周期，包括：
 * - 初始化Qt应用
 * - 创建和管理数据模型
 * - 管理UI界面
 * - 处理应用事件
 */
class Application : public QObject {
    Q_OBJECT

public:
    explicit Application(int argc, char* argv[], QObject* parent = nullptr);
    ~Application();

    /**
     * @brief 初始化应用程序
     * @return 初始化是否成功
     */
    bool initialize();

    /**
     * @brief Register handlers of the message from core
     *
     */
    void RegisterCoreMsgHandlers();

    /**
     * @brief 运行应用程序
     * @return 应用程序退出码
     */
    int run();

    /**
     * @brief 获取数据管理器
     * @return 数据管理器指针
     */
    DataManager* getDataManager() const { return m_dataManager; }

    /**
     * @brief 获取UI管理器
     * @return UI管理器指针
     */
    UIManager* getUIManager() const { return m_uiManager; }

    /**
     * @brief 获取QML引擎
     * @return QML引擎指针
     */
    QQmlApplicationEngine* getQmlEngine() const { return m_qmlEngine; }

signals:
    /**
     * @brief 应用初始化完成信号
     */
    void initialized();

    /**
     * @brief 应用即将退出信号
     */
    void aboutToQuit();

private slots:
    /**
     * @brief 处理应用初始化完成
     */
    void onInitialized();

    /**
     * @brief 处理应用即将退出
     */
    void onAboutToQuit();

private:
    /**
     * @brief 初始化Qt应用
     * @return 是否成功
     */
    bool initializeQtApp();

    /**
     * @brief 初始化数据模型
     * @return 是否成功
     */
    bool initializeModels();

    /**
     * @brief 初始化QML引擎
     * @return 是否成功
     */
    bool initializeQmlEngine();

    /**
     * @brief 注册模型到QML上下文
     */
    void registerModelsToQml();

    /**
     * @brief 加载QML文件
     * @return 是否成功
     */
    bool loadQmlFiles();

    /**
     * @brief 设置应用样式
     */
    void setupApplicationStyle();

    /**
     * @brief 设置应用信息
     */
    void setupApplicationInfo();

private:
    QGuiApplication* m_app;             ///< Qt应用程序实例
    QQmlApplicationEngine* m_qmlEngine; ///< QML引擎
    DataManager* m_dataManager;         ///< 数据管理器
    UIManager* m_uiManager;             ///< UI管理器
    CoreAgent* coreagent_{nullptr};     /* Communication with core engine. */

    // 数据模型
    CandlestickModel* m_candlestickModel;       ///< K线数据模型
    TickTradeModel* m_tickTradeModel;           ///< 逐笔成交模型
    OrderBookModel* m_orderBookModel;           ///< 买卖档位模型
    FootprintModel* m_footprintModel;           ///< 足迹图模型
    MicroOrderBookModel* m_microOrderBookModel; ///< 微盘口模型

    int m_argc;    ///< 命令行参数数量
    char** m_argv; ///< 命令行参数数组
};

} // namespace btra::gui

#endif // APPLICATION_H