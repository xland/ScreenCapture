#pragma once
#include <QSystemTrayIcon>
#include <QObject>

class Tray : public QObject
{
	Q_OBJECT

public:
	Tray(const QJsonObject& obj, const QString& lang,QObject *parent=nullptr);
	~Tray();
	void onTrayActived(QSystemTrayIcon::ActivationReason reason);
private:
};
