#pragma once
#include <Windows.h>
#include <string>
#include <memory>
#include <vector>

class WindowNative
{
public:
	static void Init();
private:
	void createWindow();
	static LRESULT CALLBACK routeWinMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
private:
};

