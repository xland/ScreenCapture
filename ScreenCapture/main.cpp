#include <QtWidgets/QApplication>
#include <QFontDatabase>
#include "WindowNative.h"
#include "CanvasWidget.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFontDatabase::addApplicationFont(":/res/iconfont.ttf");
    WindowNative::Init();
    CanvasWidget::Init();
    return a.exec();
}
