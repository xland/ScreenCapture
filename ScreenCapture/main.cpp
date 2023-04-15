//#include <QGuiApplication>
//#include <QQmlApplicationEngine>
//
//int main(int argc, char *argv[])
//{
//#if defined(Q_OS_WIN)
//    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
//#endif
//
//    QGuiApplication app(argc, argv);
//
//    QQmlApplicationEngine engine;
//    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
//    if (engine.rootObjects().isEmpty())
//        return -1;
//
//    return app.exec();
//}


#include <QApplication>
#include <QQmlApplicationEngine>
#include "ScreenImageProvider.h"
#include "ScreenShoter.h"
#include "Cutter.h"

int main(int argc, char* argv[])
{
    //    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::Floor);
    //    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
    //    qputenv("QT_SCALE_FACTOR", "1");
    QApplication  app(argc, argv);
    QQmlApplicationEngine engine;
    ScreenShoter::Init(&app);
    Cutter::Init(&app);
    qmlRegisterSingletonType<Cutter>("ScreenCapture.Cutter", 1, 0, "Cutter", [](QQmlEngine* engine, QJSEngine* scriptEngine) {
        return Cutter::Get();
        });
    engine.addImageProvider(QLatin1String("ScreenImage"), new ScreenImageProvider);
    const QUrl url("qrc:/main.qml");
    engine.load(url);
    return app.exec();
}
