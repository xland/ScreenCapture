#pragma once
#include <string>
#include <memory>
#include <vector>
#include <QFont>
#include <QFontDatabase>

class Config
{
public:
	static void Init();
	static Config* Get();
	static QFont* GetIconFont();
public:
private:
	static void initFont(const QJsonObject& obj);
};

