#pragma once
#include <string>
#include <memory>
#include <vector>

class Config
{
public:
	static void Init();
	static Config* Get();
public:

private:
	static void initFont(const QJsonObject& obj);
	static void initToolMain(const QJsonArray& arr);
	static void initToolSub(const QJsonObject& obj);
};

