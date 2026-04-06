#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>

#include "AppController.h"
#include "MessageListModel.h"

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);
    app.setApplicationName("TLOG Parser");
    app.setApplicationVersion("1.0");

    QQuickStyle::setStyle("Basic");

    qmlRegisterUncreatableType<MessageListModel>(
        "TlogViewer", 1, 0, "MessageListModel",
        "MessageListModel is created by AppController");

    AppController controller;

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("controller", &controller);

    using namespace Qt::StringLiterals;
    const QUrl url(u"qrc:/TlogViewer/qml/main.qml"_s);
    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreationFailed,
        &app, [](const QUrl& url) {
            qCritical() << "Failed to create QML object from" << url;
            QCoreApplication::exit(1);
        },
        Qt::QueuedConnection);

    engine.load(url);
    return app.exec();
}
