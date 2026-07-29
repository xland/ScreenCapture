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
	std::vector<std::pair<std::wstring, std::wstring>> getSupportedLang();
public:
private:
	Lang();
	void initLang(const std::wstring& langCode);
	void load(const std::wstring& lang);
private:
	JsonObject langObj;
};
