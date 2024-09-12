#pragma once
#include <Windows.h>
#include <string>
#include <memory>
#include <vector>

class WindowNative
{
public:
	static void Init();
	static WindowNative* Get();
public:
	HWND hwnd{ nullptr };
	int x{ 100 }, y{ 100 }, w{ 580 }, h{ 580 };
private:
	void createWindow();
	static LRESULT CALLBACK routeWinMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT processNativeMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void shotScreen();
	void getScreenRects();
	void initPosSize();
private:
	std::vector<RECT> screenRects;
	std::vector<unsigned char> winPix;
};

