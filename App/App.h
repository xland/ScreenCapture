#pragma once
#include <QFont>
#include <QJsonObject>


class App
{
public:
	static void init();
	static QFont* getIconFont();
	static QList<QRect>* getScreens();
	static void dispose();
	static void start();
public:
private:
	static void initConfig();
	static void initFont(const QJsonObject& obj);
	static void initHotKey(const QJsonObject& obj);
	static void initTool(const QJsonObject& obj,const QString& lang);
	static void initTray(const QJsonObject& obj, const QString& lang);
	static void initScreens();
};

