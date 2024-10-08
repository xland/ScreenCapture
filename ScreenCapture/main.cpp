#include <QtWidgets/QApplication>
#include "WindowNative.h"
#include "CanvasWidget.h"

#include "Config.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Config::Init();
    WindowNative::Init();
    CanvasWidget::Init();
    return a.exec();
}
