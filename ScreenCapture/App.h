#pragma once
#include "pch.h"
class WinBase;
class App
{
public:
	App(HINSTANCE hInstance);
	~App();
	static void init(HINSTANCE hInstance);
	static App* get();
	static void exit(const int& code);
	static void initDevice();
	static void makeDC(WinBase* win);
	static ID2D1Factory1* getD2D();
	static IDWriteFactory5* getWriter();
	/// <summary>
	/// 是否允许撕裂呈现（允许的话效果稍弱，但渲染更快，咱这个应用尽可能的允许）
	/// </summary>
	static BOOL allowTearing;
public:
	HINSTANCE hInstance;
private:
};

