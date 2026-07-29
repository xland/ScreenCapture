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
	void setShortcutKey(const std::wstring& type, const std::vector<std::wstring>& keys);
	std::wstring getShortcutKey(const std::wstring& type);
	void setAutoStart(bool autoStart);
	bool getAutoStart();
	std::wstring getLang();
	void setLang(const std::wstring& lang);
	void initShortcutKeys();
private:
	Setting();
	std::filesystem::path initDataPath();
	void save();
private:
	const std::filesystem::path dataPath;
	JsonObject configObj;
};

