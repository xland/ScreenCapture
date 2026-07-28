#pragma once
#include <include/Ling.h>
#include <filesystem>
#include <winrt/Windows.Data.Json.h>
using namespace winrt::Windows::Data::Json;

class Setting
{
public:
	~Setting();
	static void init();
	static Setting* get();
	std::filesystem::path getDataPath();
	const JsonObject getConfigObj();
	void setKeys(const int& type, const std::vector<std::wstring>& keys);
	void setAutoStart(bool autoStart);
	bool getAutoStart();
	std::wstring getLanguage();
	void setLanguage(const std::wstring& lang);
private:
	Setting();
	std::filesystem::path initDataPath();
	void save();
private:
	const std::filesystem::path dataPath;
	JsonObject configObj;
};

