#pragma once
#include <string>
#include <map>
class Cmd
{
public:
	Cmd();
	~Cmd();
	static void Init(const std::wstring& cmdLine);
	static Cmd* Get();
	std::wstring GetVal(const std::wstring& key);
	std::map<std::wstring,std::wstring> args;

private:
};