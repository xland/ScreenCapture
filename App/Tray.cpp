#include <qmenu.h>
#include <QJsonObject>
#include <QJsonValue>
#include <qapplication.h>
#include <qjsonarray.h>



#include "Tray.h"
#include "App.h"
#include "About.h"


Tray::Tray(QObject *parent) : QObject(parent)
{
    
}

Tray::~Tray()
{
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

void Tray::initData(const QJsonObject& obj, const QString& lang)
{
    auto tray = new Tray(qApp);
    tray->trayIcon = new QSystemTrayIcon(tray);
    tray->trayIcon->setIcon(QIcon(obj["iconPath"].toString()));
    connect(tray->trayIcon, &QSystemTrayIcon::activated, tray, &Tray::onTrayActived);
    tray->trayIcon->show();
    tray->menu = new QMenu();
    QJsonArray arr = obj["menu"].toArray();
    for (const QJsonValue& val : arr)
    {
        QAction* action = new QAction(val[lang].toString(), tray->menu);
        auto name = val["name"].toString();
        if (name == "quit") {
            connect(action, SIGNAL(triggered()), qApp, SLOT(quit()));
        }
        else if (name == "about") {
            connect(action, &QAction::triggered, [tray]() {
                if (!tray->about) {
                    tray->about = new About();
                    tray->about->setWindowIcon(tray->trayIcon->icon());
                }
                tray->about->show();
                tray->about->raise();
                });
        }
        else if (name == "start") {
            connect(action, &QAction::triggered, []() {
                App::start();
                });
        }
        connect(action, &QAction::hovered, []() {
            QGuiApplication::setOverrideCursor(Qt::ArrowCursor);
            });
        tray->menu->addAction(action);
    }
    tray->trayIcon->setContextMenu(tray->menu);
}
