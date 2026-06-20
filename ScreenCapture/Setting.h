#pragma once
class Setting
{
public:
	Setting();
	~Setting();
	static void init();
	static std::filesystem::path getDataPath();
	static const JsonObject& getConfigObj();
	static void setKeys(const int& type, const std::vector<std::wstring>& keys);
private:
	std::filesystem::path initDataPath();
	void initSettings();
	void save();
private:
	const std::filesystem::path dataPath;
	JsonObject configObj;
};

