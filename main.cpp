#include <QGuiApplication>
#include <QIcon>
#include <QImage>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QQuickWindow>
#include <QTimer>
#include "core/search/searchcontroller.h"
#include "core/search/themeiconprovider.h"

int main(int argc, char *argv[])
{
    QGuiApplication::setApplicationName(QStringLiteral("NOX"));
    QGuiApplication::setOrganizationName(QStringLiteral("NOX"));
    QGuiApplication::setDesktopFileName(QStringLiteral("NOX"));

    QGuiApplication app(argc, argv);
    QQuickStyle::setStyle(QStringLiteral("Basic"));
    const QIcon appIcon(QStringLiteral(":/Nox/resources/icons/nox.svg"));
    app.setWindowIcon(appIcon);

    const QStringList arguments = app.arguments();
    QString screenshotPath;
    const int screenshotIndex = arguments.indexOf(QStringLiteral("--screenshot"));
    if (screenshotIndex >= 0 && screenshotIndex + 1 < arguments.size()) {
        screenshotPath = arguments.at(screenshotIndex + 1);
    }

    SearchController controller;

    QQmlApplicationEngine engine;
    engine.addImageProvider(QStringLiteral("icons"), new ThemeIconProvider());
    engine.rootContext()->setContextProperty(QStringLiteral("controller"), &controller);

    engine.load(QUrl(QStringLiteral("qrc:/Nox/ui/qml/Main.qml")));
    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    if (auto *window = qobject_cast<QQuickWindow *>(engine.rootObjects().constFirst())) {
        window->setIcon(appIcon);
    }

    if (!screenshotPath.isEmpty()) {
        auto *window = qobject_cast<QQuickWindow *>(engine.rootObjects().constFirst());
        if (!window) {
            return -1;
        }

        QTimer::singleShot(300, &app, [window, screenshotPath]() {
            const QImage image = window->grabWindow();
            image.save(screenshotPath);
            QCoreApplication::quit();
        });
    }

    return app.exec();
}
