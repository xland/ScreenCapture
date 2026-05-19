#pragma once
#include "pch.h"
class WinToolBase
{
public:
	WinToolBase();
	~WinToolBase();
	void createWindow(int btnCount);
	void hide();
	void close();
	virtual void show() = 0;
public:
	HWND hwnd;
	int x, y, w, h;
	float dpi;
protected:
	virtual void onPaint() = 0;
	virtual void onClick() = 0;
protected:
	ComPtr<ID2D1SolidColorBrush> brushBg;
	ComPtr<ID2D1SolidColorBrush> brushIcon;
	ComPtr<ID2D1SolidColorBrush> brushIconHover;
	ComPtr<ID2D1SolidColorBrush> brushSpliter;
	ComPtr<ID2D1HwndRenderTarget> render;
	int hoverIndex{ -1 };
	bool mouseIn{ false };
	float btnW{ 36.f }, btnH{ 32.f }, btnPadding;
private:
	void onMouseLeave();
	void paint();
	void onMouseMove(const int& x, const int& y);
	void onKeyDown(const TCHAR& key);
	static LRESULT CALLBACK winMsg(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

