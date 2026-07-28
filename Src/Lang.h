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
	static Lang* get();
	static std::wstring get(const std::wstring& keyPath);
	const std::wstring& getLang();
	void setLang(const std::wstring& langCode);
	std::vector<std::pair<std::wstring, std::wstring>> getSupportedLang();
public:
	JsonObject langObj;
private:
	Lang();
	void load(const std::wstring& lang);
	std::wstring langCode{ L"zh-CN" };
};
