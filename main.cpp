#include <QtWidgets/QApplication>

#include "App/App.h"


int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
    App::init(); //配置文件在这里读取
    auto resut = a.exec();
    App::dispose();
    return resut;
}
