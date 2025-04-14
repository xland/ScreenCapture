#pragma once
#include <QObject>
#include <QFont>
#include <QFontDatabase>
class Font
{
public:
	static void init();
	static QFont* icon();
};

