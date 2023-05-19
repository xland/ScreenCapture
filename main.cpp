
#include "MainWindow.h"
#include "ScreenShoter.h"
#include "Icon.h"
#include <QApplication>


int main(int argc, char* argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
    QApplication a(argc, argv);
    Icon::Init();
    ScreenShoter::Init(&a);
    MainWindow w;
    w.show();
    return a.exec();
}
