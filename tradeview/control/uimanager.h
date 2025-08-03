#ifndef UIMANAGER_H
#define UIMANAGER_H

#include <QObject>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QWindow>

namespace btra::gui {

/**
 * @brief UI管理器类
 *
 * 负责管理用户界面的创建、配置和交互，包括：
 * - QML引擎管理
 * - 界面组件注册
 * - 主题和样式设置
 * - 窗口管理
 */
class UIManager : public QObject {
    Q_OBJECT

public:
    explicit UIManager(QObject* parent = nullptr);
    ~UIManager();

    /**
     * @brief 初始化UI管理器
     * @param qmlEngine QML引擎指针
     * @return 初始化是否成功
     */
    bool initialize(QQmlApplicationEngine* qmlEngine);

    /**
     * @brief 设置应用样式
     * @param style 样式名称
     */
    Q_INVOKABLE void setApplicationStyle(const QString& style);

    /**
     * @brief 设置主题
     * @param theme 主题名称
     */
    Q_INVOKABLE void setTheme(const QString& theme);

    /**
     * @brief 设置字体大小
     * @param size 字体大小
     */
    Q_INVOKABLE void setFontSize(int size);

    /**
     * @brief 注册QML组件
     * @param componentName 组件名称
     * @param componentPath 组件路径
     */
    Q_INVOKABLE void registerQmlComponent(const QString& componentName, const QString& componentPath);

    /**
     * @brief 加载QML文件
     * @param filePath 文件路径
     * @return 是否成功
     */
    Q_INVOKABLE bool loadQmlFile(const QString& filePath);

    /**
     * @brief 获取主窗口
     * @return 主窗口指针
     */
    QWindow* getMainWindow() const;

    /**
     * @brief 设置窗口标题
     * @param title 窗口标题
     */
    Q_INVOKABLE void setWindowTitle(const QString& title);

    /**
     * @brief 设置窗口大小
     * @param width 宽度
     * @param height 高度
     */
    Q_INVOKABLE void setWindowSize(int width, int height);

    /**
     * @brief 设置窗口位置
     * @param x X坐标
     * @param y Y坐标
     */
    Q_INVOKABLE void setWindowPosition(int x, int y);

    /**
     * @brief 显示窗口
     */
    Q_INVOKABLE void showWindow();

    /**
     * @brief 隐藏窗口
     */
    Q_INVOKABLE void hideWindow();

    /**
     * @brief 最小化窗口
     */
    Q_INVOKABLE void minimizeWindow();

    /**
     * @brief 最大化窗口
     */
    Q_INVOKABLE void maximizeWindow();

    /**
     * @brief 恢复窗口
     */
    Q_INVOKABLE void restoreWindow();

signals:
    /**
     * @brief 窗口状态改变信号
     * @param state 窗口状态
     */
    void windowStateChanged(int state);

    /**
     * @brief 主题改变信号
     * @param theme 主题名称
     */
    void themeChanged(const QString& theme);

    /**
     * @brief 样式改变信号
     * @param style 样式名称
     */
    void styleChanged(const QString& style);

private slots:
    /**
     * @brief 处理对象创建完成
     * @param obj 创建的对象
     * @param url 对象URL
     */
    void onObjectCreated(QObject* obj, const QUrl& url);

private:
    /**
     * @brief 初始化默认样式
     */
    void initializeDefaultStyle();

    /**
     * @brief 初始化默认主题
     */
    void initializeDefaultTheme();

    /**
     * @brief 注册内置组件
     */
    void registerBuiltinComponents();

    /**
     * @brief 设置窗口属性
     */
    void setupWindowProperties();

private:
    QQmlApplicationEngine* m_qmlEngine;            ///< QML引擎
    QWindow* m_mainWindow;                         ///< 主窗口
    QString m_currentStyle;                        ///< 当前样式
    QString m_currentTheme;                        ///< 当前主题
    int m_fontSize;                                ///< 字体大小
    QMap<QString, QString> m_registeredComponents; ///< 已注册的组件
};

} // namespace btra::gui

#endif // UIMANAGER_H