#pragma once
class Setting
{
public:
	Setting();
	~Setting();
	static void init();
	static std::filesystem::path getDataPath();
	static const std::vector<UINT>& getCapKeys();
	static const std::vector<UINT>& getLongKeys();
	static const std::vector<UINT>& getVideoKeys();
	static std::wstring getCapKeysStr();
	static std::wstring getLongKeysStr();
	static std::wstring getVideoKeysStr();
	static void setKeys(const int& type, const std::vector<UINT>& keys);
private:
	std::filesystem::path initDataPath();
	void initSettings();
	void initShortcutKeys(const JsonObject& jsonObj);
private:
	const std::filesystem::path dataPath;
	std::vector<UINT> capKeys;
	std::vector<UINT> longKeys;
	std::vector<UINT> videoKeys;
};

