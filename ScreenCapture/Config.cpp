#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QFontDatabase>

#include "Config.h"
#include "ToolMain.h"
#include "ToolSub.h"

namespace {
    std::unique_ptr<Config> setting;
}

void Config::Init()
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
    initFont(jsonObject);
    initToolMain(jsonObject["toolMain"].toArray());
    initToolSub(jsonObject["toolSub"].toObject());
}

Config* Config::Get()
{
    return Config::Get();
}

void Config::initFont(const QJsonObject& obj)
{
    QString name = obj["iconFont"].toString();
    auto result = QFontDatabase::addApplicationFont(name);
    if (result != 0) {
        qWarning() << "没有找到字体图标文件";
    }
}

void Config::initToolMain(const QJsonArray& arr)
{
    ToolMain::Init();
    auto toolMain = ToolMain::Get();
    for (const QJsonValue& val : arr)
    {
        ToolBtn btn;
        btn.name = val["name"].toString();
        btn.en = val["en"].toString("");
        btn.zhcn = val["zhcn"].toString("");
        btn.selected = val["selectDefault"].toBool(false);
        {
            bool ok;
            uint codePoint = val["icon"].toString().toUInt(&ok, 16);
            if (ok) {
                btn.icon = QChar(codePoint);
            }
            else {
                qWarning() << "转换失败";
            }
        }
        toolMain->btns.push_back(btn);
    }
}

void Config::initToolSub(const QJsonObject& obj)
{
    ToolSub::Init();
    auto toolSub = ToolSub::Get();

}
