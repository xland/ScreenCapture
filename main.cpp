#include <QtWidgets/QApplication>
#include <QtConcurrent>
#include <QPainter>
#include "App/App.h"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);  
    App::init();
    auto resut = a.exec();
    App::dispose();
    return resut;
}
