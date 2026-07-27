#pragma once
#include <include/Ling.h>

class Tray
{
public:
	~Tray();
	static void init();
	static Tray* get();
	static bool secondIns();
	void setTray(bool flag);
	void reloadHotKeys();
public:
private:
	Tray();
	void unregHotKeys();
	void regHotKeys();
	void onTrayRightClick();
	static LRESULT CALLBACK wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
private:
};

