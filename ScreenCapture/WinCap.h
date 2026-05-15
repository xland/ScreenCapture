#pragma once
#include "pch.h"
class WinCap
{
public:
	WinCap();
	~WinCap();
	static void init();
public:
	int x, y, w, h;
	float dpi;
	HWND hwnd;
private:
	void initPosSize();
	void createWindow();
	void captureScreen();
	static LRESULT CALLBACK winMsg(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void onPaint();
	void paintShape(ID2D1RenderTarget* render);
private:
	winrt::com_ptr<ID2D1SolidColorBrush> shapeBrush;
	winrt::com_ptr<ID2D1Bitmap1> screenImg;
	winrt::com_ptr<ID2D1HwndRenderTarget> render;
};

