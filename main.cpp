#include <QtWidgets/QApplication>

#include "App.h"


int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    App::Init();
    auto resut = a.exec();
    App::Dispose();
    return resut;
}
