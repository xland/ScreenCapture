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
private:
	std::filesystem::path initDataPath();
	void initSettings();
	std::wstring keyToStr(UINT vkCode);
	UINT strToKey(const std::wstring& keyName);
private:
	const std::filesystem::path dataPath;
	std::vector<UINT> capKeys;
	std::vector<UINT> longKeys;
	std::vector<UINT> videoKeys;
};

