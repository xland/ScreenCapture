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
	// 决定配置文件用哪一份：exe 同目录有 config.json 就用它（绿色版，配置跟着程序走），
	// 否则用 %appdata%\ScreenCapture\config.json。二者只认一个，读哪儿就写哪儿。
	std::filesystem::path initConfigPath();
	void save();
private:
	const std::filesystem::path dataPath;
	// 必须声明在 dataPath 之后：initConfigPath 找不到 exe 同目录的配置时要回落到 dataPath 上，
	// 成员按声明顺序初始化
	const std::filesystem::path configPath;
	JsonObject configObj;
};

