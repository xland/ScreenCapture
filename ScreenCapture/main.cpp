#include <QtWidgets/QApplication>
#include "WindowNative.h"
#include "CanvasWidget.h"

#include "Config.h"
#include "CutMask.h"
#include "ToolMain.h"
#include "ToolSub.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Config::Init();
    WindowNative::Init();
    CanvasWidget::Init();
    ToolMain::Init();
    ToolSub::Init();
    return a.exec();
}
