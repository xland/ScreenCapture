#pragma once
#include <string>
#include <map>
class Cmd
{
public:
	static void Init(const std::wstring& cmdLine);
	static Cmd* Get();
	std::wstring GetVal(const std::wstring& key);
	~Cmd();
	std::map<std::wstring,std::wstring> args;

private:
	Cmd();
};