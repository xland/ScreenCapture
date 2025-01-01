#include <QFile>
#include <QApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QFontDatabase>
#include <QToolTip>
#include <QSharedMemory>
#include <QMessageBox>

#include "../Lib/QHotKey/qhotkey.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/rotating_file_sink.h"

#include "App.h"
#include "Tray.h"
#include "NativeRect.h"
#include "../Win/WinFull.h"
#include "../Win/WinPin.h"

#include "../Tool/ToolMain.h"
#include "../Tool/ToolSub.h"
#include "../Tool/ColorCtrl.h"

namespace {
    std::unique_ptr<App> app;
    std::unique_ptr<QFont> iconFont;
    std::unique_ptr<QHotkey> hotkey;
    std::unique_ptr<Tray> tray;
    QList<QRect> screens;
    QList<QRect> windows;
    QSharedMemory* smSingleApp;
}
void App::init()
{
    //qApp->setCursorFlashTime(0);    
    QFont font("Microsoft YaHei",9);
    qApp->setFont(font);
    QFont tooltipFont("Arial", 9);
    QToolTip::setFont(tooltipFont);
    initLog();
    initConfig();
    start();
}
QFont* App::getIconFont()
{
    return iconFont.get();
}
void App::dispose()
{
    tray.reset();
    hotkey.reset();
    iconFont.reset();
    app.reset();
    if (smSingleApp->isAttached()) {
        smSingleApp->detach();
    }        
}
void App::start()
{
    NativeRect::init();
    WinFull::init();
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
        qWarning() << "无法打开文件:" << file.errorString();
        return;
    }
    QByteArray jsonData = file.readAll();
    file.close();
    QJsonDocument document = QJsonDocument::fromJson(jsonData);
    if (!document.isObject()) {
        qWarning() << "JSON不是一个对象";
        return;
    }
    QJsonObject jsonObject = document.object();
    auto lang = jsonObject["defaultLang"].toString();
    initHotKey(jsonObject);
    initFont(jsonObject);
    initTool(jsonObject, lang);
    initTray(jsonObject, lang);
    initPin(jsonObject, lang);
}
void App::initFont(const QJsonObject& obj)
{
    if (obj["icon"].isObject()) {
        auto icon = obj["icon"].toObject();
        QString fontPath = icon["fontPath"].toString();
        auto result = QFontDatabase::addApplicationFont(fontPath);
        if (result != 0) {
            qWarning() << "没有找到字体图标文件";
        }
        auto fontName = icon["fontName"].toString();
        iconFont = std::make_unique<QFont>(fontName);
        iconFont->setStyleStrategy(QFont::PreferAntialias);
        iconFont->setHintingPreference(QFont::PreferNoHinting);
    }
}
void App::initHotKey(const QJsonObject& obj)
{
    if (obj["hotKey"].isString()) {
        auto keyComb = obj["hotKey"].toString();
        hotkey = std::make_unique<QHotkey>(QKeySequence(keyComb), true);
        QObject::connect(hotkey.get(), &QHotkey::activated, &App::start);
    }
}
void App::initTool(const QJsonObject& obj, const QString& lang)
{
    if (obj["toolMain"].isArray()) {
        ToolMain::initData(obj["toolMain"].toArray(), lang);
    }
    if (obj["toolSub"].isObject()) {
        ToolSub::initData(obj["toolSub"].toObject(), lang);
    }
    if (obj["colorCtrl"].isObject()) {
        ColorCtrl::InitData(obj["colorCtrl"].toObject(), lang);
    }
}
void App::initTray(const QJsonObject& obj, const QString& lang)
{
    if (obj["tray"].isObject()) {
        tray = std::make_unique<Tray>(obj["tray"].toObject(), lang);
    }
}
void App::initPin(const QJsonObject& obj, const QString& lang)
{
    if (obj["winPin"].isArray()) {
        WinPin::initData(obj["winPin"].toArray(), lang);
    }
}

void App::initLog()
{
    auto logger = spdlog::rotating_logger_mt("logger", "Logs/log.txt", 1024000, 2); //1000K,最多2个文件
    spdlog::set_default_logger(logger);
    spdlog::set_level(spdlog::level::trace);
    spdlog::flush_on(spdlog::level::trace);
    spdlog::info("logger setted");
}
