#include <qmenu.h>
#include <QJsonObject>
#include <QJsonValue>
#include <qapplication.h>
#include <qjsonarray.h>


#include "Tray.h"
#include "App.h"

namespace{
    QMenu* menu{nullptr};
}


Tray::Tray(const QJsonObject& obj, const QString& lang, QObject *parent) : QObject(parent)
{
    auto iconPath = obj["iconPath"].toString();
    auto trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(iconPath));
    connect(trayIcon, &QSystemTrayIcon::activated, this, &Tray::onTrayActived);
    trayIcon->show();

    menu = new QMenu();
    QJsonArray arr = obj["menu"].toArray();
    for (const QJsonValue& val:arr)
    {
        QAction* action = new QAction(val[lang].toString(), menu);
        auto name = val["name"].toString();
        if (name == "quit") {
            connect(action, SIGNAL(triggered()), qApp, SLOT(quit()));
        }
        else if (name == "about") {
            connect(action, SIGNAL(triggered()), qApp, SLOT(quit()));
        }
        menu->addAction(action);
    }
    trayIcon->setContextMenu(menu);
}

Tray::~Tray()
{
    delete menu;
}

void Tray::onTrayActived(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
        case QSystemTrayIcon::Trigger:
        {
            break;
        }
        case QSystemTrayIcon::DoubleClick:
        {
            App::Start();
            break;
        }
        default:
            break;
    }
}
