#pragma once
#include <include/Ling.h>
#include <vector>
#include <winrt/Windows.Data.Json.h>
using namespace winrt::Windows::Data::Json;

class Lang
{
public:
	~Lang();
	static void init();
	// 同 Setting::dispose：langObj 也是 WinRT 对象，得趁 COM 还活着放掉
	static void dispose();
	static Lang* get();
	static std::wstring get(const std::wstring& keyPath);
	std::vector<std::pair<std::wstring, std::wstring>> getSupportedLang();
	void initLang(const std::wstring& langCode);
public:
private:
	Lang();
	void load(const std::wstring& lang);
private:
	JsonObject langObj;
};
