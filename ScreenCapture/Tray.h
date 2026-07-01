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
	void setTray(bool flag);
	void reloadHotKeys();
public:
private:
	void unregisterHotKeys();
	void regOneHotKey(const std::wstring& key,const int& msgKey, std::wstring& errMsg);
	void createWin();
	void createTray();
	void onTrayClick();
	void onTrayRightClick();
	static LRESULT CALLBACK wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
private:
	std::unique_ptr<NOTIFYICONDATA> tray;
	HWND hwnd;
};

