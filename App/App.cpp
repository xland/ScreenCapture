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
#include "Lang.h"
#include "../Win/WinFull.h"
#include "../Win/WinPin.h"
#include "../Tool/ToolMain.h"
#include "../Tool/ToolSub.h"
#include "../Tool/ColorCtrl.h"

namespace {
    std::unique_ptr<App> app; 
}
void App::init()
{
    QFont font("Microsoft YaHei", 9);
    qApp->setFont(font);
    Lang::init("ZhCn");
    NativeRect::init();
    WinFull::init();
}
void App::dispose()
{
    app.reset();       
}
