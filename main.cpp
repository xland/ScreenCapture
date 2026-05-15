#include <QtWidgets/QApplication>
#include "App/App.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    App::init();
    auto result = a.exec();
    App::dispose();
    return result;
}
