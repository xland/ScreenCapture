#pragma once
#include "pch.h"
class App
{
public:
	App();
	~App();
	static void init();
	static App* get();
public:
	winrt::Windows::System::DispatcherQueue dq;
	std::vector<std::wstring> toolBtns;
private:
	static void initDQ();
	void procCmd();
	void initToolBtns(const std::wstring_view& argv);

};

