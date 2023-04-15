#include <QApplication>
#include <QQmlApplicationEngine>
#include "ScreenImageProvider.h"
#include "ScreenShoter.h"
#include "Cutter.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
    QApplication  app(argc, argv);
    QQmlApplicationEngine engine;
    ScreenShoter::Init(&app);
    Cutter::Init(&app);
    qmlRegisterSingletonType<Cutter>("ScreenCapture.Cutter", 1, 0, "Cutter", [](QQmlEngine* engine, QJSEngine* scriptEngine){
        return Cutter::Get();
    });
    engine.addImageProvider(QLatin1String("ScreenImage"), new ScreenImageProvider);
    const QUrl url("qrc:/main.qml");
    engine.load(url);
    return app.exec();
}
