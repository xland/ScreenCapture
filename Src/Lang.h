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
	// 用第三方语言文件时才有：内置的 en-US，给 Lang::get 兜底用。
	// 内置语言（zh-CN / en-US）本身就是全的，这时它是空的（nullptr）
	JsonObject fallbackObj{ nullptr };
};
