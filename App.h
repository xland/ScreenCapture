#pragma once
#include <string>
#include <memory>
#include <vector>
#include <QFont>
#include <QFontDatabase>

class App
{
public:
	static void Init();
	static App* Get();
	static QFont* GetIconFont();
	static void Start();
	static void Dispose();
public:
private:
	static void initConfig();
	static void initFont(const QJsonObject& obj);
	static void initHotKey(const QJsonObject& obj);
	static void initTool(const QJsonObject& obj,const QString& lang);
	static void initTray(const QJsonObject& obj, const QString& lang);
};

