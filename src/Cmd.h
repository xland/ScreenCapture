#pragma once
#include <string>
#include <vector>
class Cmd
{
public:
	static void Init(const std::wstring& cmdLine);
	static Cmd* Get();
	~Cmd();
	std::vector<std::wstring> args;
private:
	Cmd();
};