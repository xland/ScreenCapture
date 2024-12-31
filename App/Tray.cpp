#include <qmenu.h>
#include <QJsonObject>
#include <QJsonValue>
#include <qapplication.h>
#include <qjsonarray.h>



#include "Tray.h"
#include "App.h"
#include "About.h"

namespace{
    QMenu* menu{nullptr};
    About* about{ nullptr };
    QIcon* icon{ nullptr };
}


Tray::Tray(const QJsonObject& obj, const QString& lang, QObject *parent) : QObject(parent)
{
    auto iconPath = obj["iconPath"].toString();
    icon = new QIcon(iconPath);
    auto trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(*icon);
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
            connect(action, &QAction::triggered, []() {
                if (!about) {
                    about = new About();
                    about->setWindowIcon(*icon);
                }
                about->show();
                about->raise();
                });
        }
        connect(action, &QAction::hovered, []() {
            QGuiApplication::setOverrideCursor(Qt::ArrowCursor);
        });
        menu->addAction(action);
    }
    trayIcon->setContextMenu(menu);
}

Tray::~Tray()
{
    delete icon;
    delete menu;
    delete about;
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
            App::start();
            break;
        }
        default:
            break;
    }
}
