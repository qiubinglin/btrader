#include "uimanager.h"

#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QWindow>
#include <QDebug>

UIManager::UIManager(QObject *parent)
    : QObject(parent)
    , m_qmlEngine(nullptr)
    , m_mainWindow(nullptr)
    , m_currentStyle("Basic")
    , m_currentTheme("dark")
    , m_fontSize(12)
{
}

UIManager::~UIManager()
{
}

bool UIManager::initialize(QQmlApplicationEngine* qmlEngine)
{
    qDebug() << "Initializing UIManager...";
    
    m_qmlEngine = qmlEngine;
    
    if (!m_qmlEngine) {
        qCritical() << "QML engine is null";
        return false;
    }
    
    // 连接对象创建完成信号
    connect(m_qmlEngine, &QQmlApplicationEngine::objectCreated,
            this, &UIManager::onObjectCreated, Qt::QueuedConnection);
    
    // 初始化默认样式和主题
    initializeDefaultStyle();
    initializeDefaultTheme();
    
    // 注册内置组件
    registerBuiltinComponents();
    
    qDebug() << "UIManager initialized successfully";
    return true;
}

void UIManager::setApplicationStyle(const QString& style)
{
    if (m_currentStyle != style) {
        m_currentStyle = style;
        QQuickStyle::setStyle(style);
        emit styleChanged(style);
        qDebug() << "Application style changed to:" << style;
    }
}

void UIManager::setTheme(const QString& theme)
{
    if (m_currentTheme != theme) {
        m_currentTheme = theme;
        emit themeChanged(theme);
        qDebug() << "Theme changed to:" << theme;
    }
}

void UIManager::setFontSize(int size)
{
    if (m_fontSize != size) {
        m_fontSize = size;
        qDebug() << "Font size changed to:" << size;
    }
}

void UIManager::registerQmlComponent(const QString& componentName, const QString& componentPath)
{
    m_registeredComponents[componentName] = componentPath;
    qDebug() << "Registered QML component:" << componentName << "->" << componentPath;
}

bool UIManager::loadQmlFile(const QString& filePath)
{
    if (!m_qmlEngine) {
        qCritical() << "QML engine is null";
        return false;
    }
    
    QUrl url;
    if (filePath.startsWith("qrc:/")) {
        url = QUrl(filePath);
    } else {
        url = QUrl::fromLocalFile(filePath);
    }
    
    m_qmlEngine->load(url);
    qDebug() << "Loaded QML file:" << filePath;
    return true;
}

QWindow* UIManager::getMainWindow() const
{
    return m_mainWindow;
}

void UIManager::setWindowTitle(const QString& title)
{
    if (m_mainWindow) {
        m_mainWindow->setTitle(title);
        qDebug() << "Window title set to:" << title;
    }
}

void UIManager::setWindowSize(int width, int height)
{
    if (m_mainWindow) {
        m_mainWindow->resize(width, height);
        qDebug() << "Window size set to:" << width << "x" << height;
    }
}

void UIManager::setWindowPosition(int x, int y)
{
    if (m_mainWindow) {
        m_mainWindow->setPosition(x, y);
        qDebug() << "Window position set to:" << x << "," << y;
    }
}

void UIManager::showWindow()
{
    if (m_mainWindow) {
        m_mainWindow->show();
        qDebug() << "Window shown";
    }
}

void UIManager::hideWindow()
{
    if (m_mainWindow) {
        m_mainWindow->hide();
        qDebug() << "Window hidden";
    }
}

void UIManager::minimizeWindow()
{
    if (m_mainWindow) {
        m_mainWindow->showMinimized();
        qDebug() << "Window minimized";
    }
}

void UIManager::maximizeWindow()
{
    if (m_mainWindow) {
        m_mainWindow->showMaximized();
        qDebug() << "Window maximized";
    }
}

void UIManager::restoreWindow()
{
    if (m_mainWindow) {
        m_mainWindow->showNormal();
        qDebug() << "Window restored";
    }
}

void UIManager::onObjectCreated(QObject* obj, const QUrl& url)
{
    if (obj) {
        // 查找主窗口
        if (!m_mainWindow) {
            QWindow* window = qobject_cast<QWindow*>(obj);
            if (window) {
                m_mainWindow = window;
                setupWindowProperties();
                qDebug() << "Main window found and configured";
            }
        }
    }
}

void UIManager::initializeDefaultStyle()
{
    // 样式已在Application中设置，这里只记录当前样式
    qDebug() << "Default style set to:" << m_currentStyle;
}

void UIManager::initializeDefaultTheme()
{
    // 设置默认主题
    m_currentTheme = "dark";
    qDebug() << "Default theme set to:" << m_currentTheme;
}

void UIManager::registerBuiltinComponents()
{
    // 注册内置QML组件
    if (m_qmlEngine) {
        // 这里可以注册自定义的QML组件
        qDebug() << "Built-in components registered";
    }
}

void UIManager::setupWindowProperties()
{
    if (m_mainWindow) {
        // 设置窗口属性
        m_mainWindow->setTitle("订单流交易系统");
        m_mainWindow->resize(1400, 900);
        m_mainWindow->setMinimumSize(QSize(800, 600));
        
        qDebug() << "Window properties configured";
    }
} 