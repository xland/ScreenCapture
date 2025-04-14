#include <QFile>
#include <QApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QToolTip>
#include <QSharedMemory>
#include <QMessageBox>
#include "App.h"
#include "NativeRect.h"
#include "Font.h"
#include "Lang.h"
#include "../Win/WinFull.h"
#include "../Win/WinPin.h"
#include "../Tool/ToolMain.h"
#include "../Tool/ToolSub.h"
#include "../Tool/ColorCtrl.h"

namespace {
    std::unique_ptr<App> app; 
    QList<QRect> screens;
    QList<QRect> windows;
}
void App::init()
{
    Lang::init("ZhCn");
    Font::init();
    start();
}
void App::dispose()
{
    app.reset();       
}
void App::start()
{
    NativeRect::init();
    WinFull::init();
}
