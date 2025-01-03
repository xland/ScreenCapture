#pragma once
#include <QFont>
#include <QJsonObject>

class App
{
public:
	static void init();
	static void dispose();
	static void start();
	static bool singleAppLock();
public:
private:
	static void initConfig();
};

