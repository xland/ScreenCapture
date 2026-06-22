#pragma once
class Setting
{
public:
	Setting();
	~Setting();
	static void init();
	static void dispose();
	static std::filesystem::path getDataPath();
	static const JsonObject& getConfigObj();
	static void setKeys(const int& type, const std::vector<std::wstring>& keys);
	static void setSwitch(bool autoStart, bool showTray);
private:
	std::filesystem::path initDataPath();
	void initSettings();
	void save();
private:
	const std::filesystem::path dataPath;
	JsonObject configObj;
};

