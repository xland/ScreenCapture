#pragma once

#include <filesystem>

/// <summary>
/// 多语言管理（单例），与 Setting 同一套模式。
/// 字典直接硬编码在代码中（保持“单文件、零外部依赖”的定位）。
/// 默认/主语言为简体中文（zh-CN），额外支持 Bahasa Indonesia（id）。
/// 新增语言：在 supportedLangs 追加一项，并在 load() 补全对应字典。
/// </summary>
class Lang
{
public:
	Lang();
	~Lang();
	static void init();
	static void dispose();
	/// 按 key 取当前语言的文案；未命中返回空串
	static const std::wstring get(const std::wstring& key);
	/// 当前语言代码（zh-CN / id ...）
	static const std::wstring& getLang();
	/// 切换语言并重新加载字典（不写盘，调用方负责持久化）
	static void setLang(const std::wstring& lang);
	/// 所有支持的语言（驱动语言下拉框）
	static const std::map<std::wstring, std::wstring>& getLangs();
private:
	void load(const std::wstring& lang);
	void loadExternal(const std::wstring& l);
	static void scanLangDir();
	static std::filesystem::path getLangDir();
	std::map<std::wstring, std::wstring> dic;
	std::wstring langCode{ L"zh-CN" };
	static std::map<std::wstring, std::filesystem::path> externalFiles;
};
