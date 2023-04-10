#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <Windows.h>
#include "ScreenImageProvider.h"
#include "ScreenShoter.h"
#include "Cutter.h"

QList<QVariant> desktopWindowRects;

static BOOL CALLBACK enumWindowCallback(HWND hwnd, LPARAM lparam)
{
    if(!IsWindowVisible(hwnd)) return TRUE;
    if(IsIconic(hwnd)) return TRUE;
    RECT rect;
    GetWindowRect(hwnd,&rect);
    if(rect.top <= 0 && rect.left <= 0 && rect.bottom <= 0 && rect.right <= 0) return TRUE;
//    qDebug() << hwnd;
    return TRUE;
}

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    EnumWindows(enumWindowCallback, NULL);
    QQmlApplicationEngine engine;
    ScreenShoter::Init();
    qmlRegisterSingletonType<Cutter>("ScreenCapture.Cutter", 1, 0, "Cutter", [](QQmlEngine* engine, QJSEngine* scriptEngine){
        return new Cutter();
    });
    engine.addImageProvider(QLatin1String("ScreenImage"), new ScreenImageProvider);
    const QUrl url(u"qrc:/ScreenCapture/Main.qml"_qs);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
        &app, []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.load(url);
    return app.exec();
}
