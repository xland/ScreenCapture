#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QFontDatabase>

#include "App.h"
#include "Tool/ToolMain.h"
#include "Tool/ToolSub.h"
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
    auto keyComb = jsonObject["hotKey"].toString();
    initHotKey(keyComb);
    initFont(jsonObject);
    auto lang = jsonObject["defaultLang"].toString();
    ToolMain::InitData(jsonObject["toolMain"].toArray(), lang);
    ToolSub::InitData(jsonObject["toolSub"].toObject(), lang);

    auto trayObj = jsonObject["tray"].toObject();
    auto flag = trayObj["showIcon"].toBool();
    if (flag) {
        tray = std::make_unique<Tray>(jsonObject["tray"].toObject(), lang);
    }
    
}

void App::initFont(const QJsonObject& obj)
{
    QString name = obj["iconFont"].toString();
    auto result = QFontDatabase::addApplicationFont(name);
    if (result != 0) {
        qWarning() << "没有找到字体图标文件";
    }
    auto fontName = obj["iconFontName"].toString();
    iconFont = std::make_unique<QFont>(fontName);
    iconFont->setStyleStrategy(QFont::PreferAntialias);
    iconFont->setHintingPreference(QFont::PreferNoHinting);
}

void App::initHotKey(const QString& keyComb)
{
    hotkey = std::make_unique<QHotkey>(QKeySequence(keyComb), true);
    QObject::connect(hotkey.get(), &QHotkey::activated, &App::Start);
}
