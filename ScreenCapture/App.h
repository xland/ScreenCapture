#pragma once
#include "pch.h"
class App
{
public:
	App(HINSTANCE hInstance);
	~App();
	static void init(HINSTANCE hInstance);
	static App* get();
	static void exit(const int& code);
public:
	winrt::Windows::System::DispatcherQueue dq;
	HINSTANCE hInstance;
private:
	static void initDQ();
};

