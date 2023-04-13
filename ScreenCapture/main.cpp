#include <QGuiApplication>
#include <QApplication>
#include <QQmlApplicationEngine>
#include "ScreenImageProvider.h"
#include "ScreenShoter.h"
#include "Cutter.h"
#include <QFontDatabase>

int main(int argc, char *argv[])
{
//    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::Floor);
//    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
//    qputenv("QT_SCALE_FACTOR", "1");
    QApplication  app(argc, argv);
//    const int font_id = QFontDatabase::addApplicationFont("qrc:/font.ttf");
//    auto list = QFontDatabase::applicationFontFamilies(font_id);
//    for (int var = 0; var < list.count(); ++var) {
//        qDebug() << list[var];
//    }
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
