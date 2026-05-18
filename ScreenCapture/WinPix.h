#pragma once
#include "pch.h"
class WinPix
{
public:
	WinPix();
	~WinPix();
	void init();
	void move(const int& x,const int& y);
	void close();
public:
	float dpi;
	HWND hwnd;
	int x, y, w, h, srcW{ 50 },srcH{ 30 },cursorX,cursorY;
private:
	void createWindow();
	void initRender();
	COLORREF paintImg();
	void paintText(COLORREF& cr, const int& x, const int& y);
	void onPaint();
	static LRESULT CALLBACK winMsg(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
private:
	int scaleNum{ 5 },imgH;
	ComPtr<IDWriteTextFormat> textFormat;
	ComPtr<ID2D1HwndRenderTarget> render;
	ComPtr<ID2D1SolidColorBrush> textBrush;
	ComPtr<ID2D1SolidColorBrush> bgBrush;
	ComPtr<ID2D1SolidColorBrush> borderBrush;
	ComPtr<ID2D1SolidColorBrush> crossBrush;
	D2D1_RECT_F crossRect0;
	D2D1_RECT_F crossRect1;
	D2D1_RECT_F crossRect2;
	D2D1_RECT_F crossRect3;
};