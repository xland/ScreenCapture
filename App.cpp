#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QFontDatabase>

#include "App.h"
#include "Tool/ToolMain.h"
#include "Tool/ToolSub.h"
#include "Tool/ColorCtrl.h"
#include "QHotKey/qhotkey.h"
#include "WindowNative.h"
#include "CanvasWidget.h"
#include "Tray.h"

namespace {
    std::unique_ptr<App> setting;
    std::unique_ptr<QFont> iconFont;
    std::unique_ptr<QHotkey> hotkey;
    std::unique_ptr<Tray> tray;
}

void App::Init()
{
    initConfig();
    App::Start();
}

App* App::Get()
{
    return setting.get();
}

QFont* App::GetIconFont()
{
    return iconFont.get();
}

void App::Start()
{
    WindowNative::Init();
    CanvasWidget::Init();
}

void App::Dispose()
{
    tray.reset();
    hotkey.reset();
    iconFont.reset();
    setting.reset();
}

void App::initConfig()
{
    QFile file("./config/config.json");
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
        QObject::connect(hotkey.get(), &QHotkey::activated, &App::Start);
    }
}

void App::initTool(const QJsonObject& obj, const QString& lang)
{
    if (obj["toolMain"].isArray()) {
        ToolMain::InitData(obj["toolMain"].toArray(), lang);
    }
    if (obj["toolSub"].isObject()) {
        ToolSub::InitData(obj["toolSub"].toObject(), lang);
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
