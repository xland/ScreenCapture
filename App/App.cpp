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
#include "Tray.h"
#include "Logger.h"
#include "NativeRect.h"
#include "Font.h"
#include "../Win/Box.h"
#include "../Lib/QHotKey/qhotkey.h"
#include "../Tool/ToolMain.h"
#include "../Tool/ToolSub.h"
//#include "../Win/WinPin.h"
//#include "../Tool/ColorCtrl.h"

namespace {
    std::unique_ptr<App> app; 
    QList<QRect> screens;
    QList<QRect> windows;
    QSharedMemory* smSingleApp;
}
void App::init()
{
    new Logger(qApp);
    initConfig();
    start();
}
void App::dispose()
{
    app.reset();
    if (smSingleApp->isAttached()) {
		smSingleApp->detach();//如果是最后一个进程，则会自动删除共享内存
    }        
}
void App::start()
{
    NativeRect::init();
	Box::init();
}
bool App::singleAppLock()
{
    smSingleApp = new QSharedMemory("ScreenCapture", qApp);
    bool flag{ false };
    if (smSingleApp->attach(QSharedMemory::ReadOnly)) {
        smSingleApp->detach();
        flag = false;
    }
    else if (smSingleApp->create(1)) {
        flag = true;
    }
    if (!flag) {
        QMessageBox::warning(nullptr, "Warning", "ScreenCapture is already running.");
    }    
    return flag;
}
void App::initConfig()
{
    QFile file("./Config/config.json");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qFatal() << "打开配置文件失败";
        return;
    }
    QByteArray jsonData = file.readAll();
    file.close();
    QJsonDocument document = QJsonDocument::fromJson(jsonData);
    if (!document.isObject()) {
        qFatal() << "配置文件格式错误";
        return;
    }
    QJsonObject jsonObject = document.object();
    auto lang = jsonObject["defaultLang"].toString();
    Tray::initData(jsonObject["tray"].toObject(), lang);
    ToolMain::initData(jsonObject["toolMain"].toArray(), lang);
    ToolSub::initData(jsonObject["toolSub"].toObject(), lang);
    //WinPin::initData(jsonObject["winPin"].toArray(), lang);
    //ColorCtrl::initData(jsonObject["colorCtrl"].toObject(), lang);
    Font::initData(jsonObject["icon"].toObject());
    auto hotkey = new QHotkey(QKeySequence(jsonObject["hotKey"].toString()), true,qApp);
    QObject::connect(hotkey, &QHotkey::activated, &App::start);
}
