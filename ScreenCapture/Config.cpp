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
    ToolMain::InitData(jsonObject["toolMain"].toArray());
    ToolSub::InitData(jsonObject["toolSub"].toObject());
}

Config* Config::Get()
{
    return setting.get();
}

void Config::initFont(const QJsonObject& obj)
{
    QString name = obj["iconFont"].toString();
    auto result = QFontDatabase::addApplicationFont(name);
    if (result != 0) {
        qWarning() << "没有找到字体图标文件";
    }
}
