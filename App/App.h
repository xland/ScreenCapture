#pragma once
#include <QFont>
#include <QJsonObject>

class App
{
public:
	static void init();
	static void dispose();
	static QString getSavePath();
	static std::tuple<int, int> getCompressVal();
	static int getCustomCap();
	static QStringList getTool();
public:
private:
	static QMap<QString, QString> getCmd();
	static bool parseCmd(const QMap<QString, QString>& params);
	static void pinClipboard(const QString& cmd);
	static void pinFile(const QString& cmd);
	static void pinArea(const QString& cmd);
	static bool setCompressVal(const QString& cmd);
	static void capArea(const QString& cmd);
	static void capFullscreen(const QString& cmd);
	static bool capCustom(const QString& cmd);
	static void exit(const int& code);
	static void attachConsole();
};

