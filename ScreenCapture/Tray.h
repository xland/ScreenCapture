#pragma once
#include "pch.h"
class Tray
{
public:
	Tray();
	~Tray();
	static void init();
	static Tray* get();
	static bool secondIns();
public:
private:
	void regHotKey();
	void createWin();
	void createTray();
	void onTrayClick();
	void onTrayRightClick();
	static LRESULT CALLBACK wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
private:
	std::unique_ptr<NOTIFYICONDATA> tray;
	HWND hwnd;
};

