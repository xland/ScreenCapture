#pragma once
#include "pch.h"
class WinTool
{
public:
	WinTool();
	~WinTool();
	void createWindow();
	void hide();
	void show();
	void close();
public:
	HWND hwnd;
	int x, y, w, h;
	float dpi;
private:
	void initRender();
	IDWriteTextLayout* getIconLayout(const std::wstring& icon);
	void onMouseLeave();
	void onPaint();
	void onMouseMove(const int& x, const int& y);
	void onMouseClick();
	void changeSelectBtn(const std::wstring& name);
	void onKeyDown(const TCHAR& key);
	static LRESULT CALLBACK winMsg(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
private:
	ComPtr<ID2D1SolidColorBrush> brushBg;
	ComPtr<ID2D1SolidColorBrush> brushIcon;
	ComPtr<ID2D1SolidColorBrush> brushIconHover;
	ComPtr<ID2D1SolidColorBrush> brushSpliter;
	ComPtr<ID2D1HwndRenderTarget> render;
	int hoverIndex{ -1 };
	bool mouseIn{false};
	float btnW{ 36.f }, btnH{ 32.f }, btnPadding;
};

