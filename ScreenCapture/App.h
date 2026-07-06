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
	static void initArgs();
	static void exit(const int& code);
	static void initDevice();
	// 释放 D2D/D3D/DXGI/DWrite 设备对象
	static void disposeDevice();
	// 强制释放并重建全局设备（D3D 设备丢失时调用）
	static void recreateDevice();
	// 若当前没有任何 Win* 窗口在活动，则调用 disposeDevice()；否则 no-op。
	static void disposeDeviceIfIdle();
	static void makeDC(WinBase* win);
	static std::wstring& getArg(const std::wstring& key);
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

