#pragma once
#include "pch.h"
class Tray
{
public:
	Tray();
	~Tray();
	static void init();
	static Tray* get();
public:
private:
	void createWin();
	void createTray();
	void onTrayClick(const int&x,const int& y);
	void onTrayRightClick(const int& x, const int& y);
	static LRESULT CALLBACK wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
private:
	std::unique_ptr<NOTIFYICONDATA> tray;
	HWND hwnd;
};

