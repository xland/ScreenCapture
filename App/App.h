#pragma once
#include <QFont>
#include <QJsonObject>
#include "spdlog/spdlog.h"

class App
{
public:
	static void init();
	static QFont* getIconFont();
	static void dispose();
	static void start();
	static bool singleAppLock();
public:
private:
	static void initConfig();
	static void initFont(const QJsonObject& obj);
	static void initHotKey(const QJsonObject& obj);
	static void initTool(const QJsonObject& obj,const QString& lang);
	static void initTray(const QJsonObject& obj, const QString& lang);
	static void initPin(const QJsonObject& obj, const QString& lang);
	static void initLog();
};

