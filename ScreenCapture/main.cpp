#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "ScreenImageProvider.h"
#include "ScreenShoter.h"
#include "Cutter.h"

int main(int argc, char *argv[])
{
//    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::Floor);
//    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
//    qputenv("QT_SCALE_FACTOR", "1");
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    ScreenShoter::Init(&app);
    Cutter::Init(&app);
    qmlRegisterSingletonType<Cutter>("ScreenCapture.Cutter", 1, 0, "Cutter", [](QQmlEngine* engine, QJSEngine* scriptEngine){
        return Cutter::Get();
    });
    engine.addImageProvider(QLatin1String("ScreenImage"), new ScreenImageProvider);
    const QUrl url(u"qrc:/ScreenCapture/Main.qml"_qs);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
        &app, []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.load(url);
    return app.exec();
}
