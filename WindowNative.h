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
	static void Close();
	HWND hwnd;
	int x, y, w, h;
private:
	void createWindow();
	static LRESULT CALLBACK routeWinMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
private:
};

