#pragma once
#include <QSystemTrayIcon>
#include <QObject>

class About;
class Tray : public QObject
{
	Q_OBJECT

public:
	Tray(QObject *parent=nullptr);
	~Tray();
	void onTrayActived(QSystemTrayIcon::ActivationReason reason);
	static void initData(const QJsonObject& obj, const QString& lang);
public:
	QSystemTrayIcon* trayIcon;
	About* about;
private:
	QMenu* menu;
};
