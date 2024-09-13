#include <QtWidgets/QApplication>
#include "WindowNative.h"
#include "CanvasWidget.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    WindowNative::Init();
    return a.exec();
}
