#pragma once
#include <QFont>
#include <QJsonObject>

class App
{
public:
	static void init();
	static void dispose();
	static QString getSavePath();
public:
private:
	static bool parseCmd();
	static void pinClipboard(const QString& cmd);
	static void pinFile(const QString& cmd);
	static void pinArea(const QString& cmd);
	static void exit(const int& code);
};

