#pragma once
#include <string>
#include <memory>
#include <vector>
#include <QFont>
#include <QFontDatabase>

class WinFull;
class WinBoard;
class WinCanvas;
class WinMask;
class App
{
public:
	static void init();
	static App* get();
	static QFont* getIconFont();
	static WinFull* getFull();
	static void disposeFull();
	static void dispose();
	static void start();
public:
private:
	static void initConfig();
	static void initFont(const QJsonObject& obj);
	static void initHotKey(const QJsonObject& obj);
	static void initTool(const QJsonObject& obj,const QString& lang);
	static void initTray(const QJsonObject& obj, const QString& lang);
};

