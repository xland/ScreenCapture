
#include "MainWindow.h"
#include "ScreenShoter.h"
#include <QApplication>


int main(int argc, char* argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
    QApplication a(argc, argv);
    ScreenShoter::Init(&a);
    MainWindow w;
    w.show();
    return a.exec();
}
