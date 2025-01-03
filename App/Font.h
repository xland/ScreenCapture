#pragma once
#include <QObject>
#include <QFont>
#include <QFontDatabase>
#include <QJsonObject>
class Font
{
public:
	static void initData(const QJsonObject& obj);
	static QFont* icon();
};

