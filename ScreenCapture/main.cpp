#include <QtWidgets/QApplication>
#include "WindowNative.h"
#include "CanvasWidget.h"

#include "App.h"
#include "CutMask.h"
#include "ToolMain.h"
#include "ToolSub.h"
#include "CutMask.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    App::Init();
    auto resut = a.exec();
    App::Dispose();
    return resut;
}
