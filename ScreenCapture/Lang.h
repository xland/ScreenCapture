#pragma once

class Lang
{
public:
	Lang();
	~Lang();
	static void init();
	static void dispose();
	static const std::wstring& get(const std::wstring& key);
	static const std::wstring& getLang();
	static void setLang(const std::wstring& lang);
	static const std::map<std::wstring, std::wstring>& getLangs();
private:
	void load(const std::wstring& lang);
	std::map<std::wstring, std::wstring> dic;
	std::wstring langCode{ L"zh-CN" };
};
