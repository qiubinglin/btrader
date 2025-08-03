#include <QDebug>
#include <QDir>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>

#include "application.h"

int main(int argc, char *argv[]) {
    // 创建应用程序实例
    btra::gui::Application app(argc, argv);

    // 初始化应用程序
    if (!app.initialize()) {
        qCritical() << "Failed to initialize application";
        return -1;
    }

    // 运行应用程序
    int result = app.run();

    qDebug() << "Application exited with code:" << result;
    return result;
}