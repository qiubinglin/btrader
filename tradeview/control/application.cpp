#include "application.h"

#include <QDebug>
#include <QDir>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>

#include "datamanager.h"
#include "uimanager.h"

Application::Application(int argc, char *argv[], QObject *parent)
    : QObject(parent),
      m_app(nullptr),
      m_qmlEngine(nullptr),
      m_dataManager(nullptr),
      m_uiManager(nullptr),
      m_candlestickModel(nullptr),
      m_tickTradeModel(nullptr),
      m_orderBookModel(nullptr),
      m_footprintModel(nullptr),
      m_microOrderBookModel(nullptr),
      m_argc(argc),
      m_argv(argv) {}

Application::~Application() {
    delete m_uiManager;
    delete m_dataManager;
    delete m_qmlEngine;
    delete m_app;
}

bool Application::initialize() {
    qDebug() << "Initializing application...";

    // 初始化Qt应用
    if (!initializeQtApp()) {
        qCritical() << "Failed to initialize Qt application";
        return false;
    }

    // 初始化数据模型
    if (!initializeModels()) {
        qCritical() << "Failed to initialize data models";
        return false;
    }

    // 初始化QML引擎
    if (!initializeQmlEngine()) {
        qCritical() << "Failed to initialize QML engine";
        return false;
    }

    // 注册模型到QML上下文
    registerModelsToQml();

    // 加载QML文件
    if (!loadQmlFiles()) {
        qCritical() << "Failed to load QML files";
        return false;
    }

    // 创建数据管理器
    m_dataManager = new DataManager(this);
    m_dataManager->setModels(m_candlestickModel, m_tickTradeModel, m_orderBookModel, m_footprintModel,
                             m_microOrderBookModel);
    if (!m_dataManager->initialize()) {
        qCritical() << "Failed to initialize data manager";
        return false;
    }

    // 创建UI管理器
    m_uiManager = new UIManager(this);
    if (!m_uiManager->initialize(m_qmlEngine)) {
        qCritical() << "Failed to initialize UI manager";
        return false;
    }

    // 注册管理器到QML上下文
    m_qmlEngine->rootContext()->setContextProperty("dataManager", m_dataManager);
    m_qmlEngine->rootContext()->setContextProperty("uiManager", m_uiManager);

    // 连接数据源并订阅交易对
    m_dataManager->connectDataSource("simulation");
    m_dataManager->subscribeSymbol("BTC/USDT");
    m_dataManager->subscribeSymbol("ETH/USDT");
    m_dataManager->subscribeSymbol("BNB/USDT");

    qDebug() << "Application initialized successfully";
    emit initialized();

    return true;
}

int Application::run() {
    qDebug() << "Starting application...";

    // 连接应用退出信号
    connect(m_app, &QGuiApplication::aboutToQuit, this, &Application::onAboutToQuit);

    // 运行应用程序
    int result = m_app->exec();

    qDebug() << "Application finished with exit code:" << result;
    return result;
}

void Application::onInitialized() { qDebug() << "Application initialization completed"; }

void Application::onAboutToQuit() {
    qDebug() << "Application is about to quit";
    emit aboutToQuit();
}

bool Application::initializeQtApp() {
    // 设置应用样式（必须在创建QGuiApplication之前）
    setupApplicationStyle();

    m_app = new QGuiApplication(m_argc, m_argv);

    // 设置应用信息
    setupApplicationInfo();

    return true;
}

bool Application::initializeModels() {
    // 创建数据模型
    m_candlestickModel = new CandlestickModel(this);
    m_tickTradeModel = new TickTradeModel(this);
    m_orderBookModel = new OrderBookModel(this);
    m_footprintModel = new FootprintModel(this);
    m_microOrderBookModel = new MicroOrderBookModel(this);

    return true;
}

bool Application::initializeQmlEngine() {
    m_qmlEngine = new QQmlApplicationEngine(this);

    // 连接对象创建失败信号
    connect(
        m_qmlEngine, &QQmlApplicationEngine::objectCreated, this,
        [](QObject *obj, const QUrl &objUrl) {
            if (!obj && objUrl == QUrl(QStringLiteral("qrc:/main.qml"))) {
                QCoreApplication::exit(-1);
            }
        },
        Qt::QueuedConnection);

    return true;
}

void Application::registerModelsToQml() {
    // 注册数据模型到QML上下文
    m_qmlEngine->rootContext()->setContextProperty("candlestickModel", m_candlestickModel);
    m_qmlEngine->rootContext()->setContextProperty("tickTradeModel", m_tickTradeModel);
    m_qmlEngine->rootContext()->setContextProperty("orderBookModel", m_orderBookModel);
    m_qmlEngine->rootContext()->setContextProperty("footprintModel", m_footprintModel);
    m_qmlEngine->rootContext()->setContextProperty("microOrderBookModel", m_microOrderBookModel);
}

bool Application::loadQmlFiles() {
    // 加载主QML文件
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    m_qmlEngine->load(url);

    return true;
}

void Application::setupApplicationStyle() {
    // 设置QML样式
    QQuickStyle::setStyle("Basic");
}

void Application::setupApplicationInfo() {
    // 设置应用信息
    m_app->setApplicationName("btrader GUI");
    m_app->setApplicationVersion("1.0.0");
    m_app->setOrganizationName("BTrader");
    m_app->setOrganizationDomain("btrader.com");
}