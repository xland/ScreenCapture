#pragma once
#include "pch.h"
#include "WinBase.h"
class WinPix :public WinBase
{
public:
	WinPix(const int& x, const int& y);
	~WinPix();
	void move(const int& x,const int& y);
public:
private:
	void initRes();
	COLORREF paintImg();
	void paintText(COLORREF& cr, const int& x, const int& y);
	void onPaint() override;
	LRESULT onHitTest(WPARAM wParam, LPARAM lParam) override;
private:
	ComPtr<ID2D1SolidColorBrush> textBrush;
	ComPtr<ID2D1SolidColorBrush> bgBrush;
	ComPtr<ID2D1SolidColorBrush> borderBrush;
	ComPtr<ID2D1SolidColorBrush> crossBrush;
	D2D1_RECT_F crossRect0;
	D2D1_RECT_F crossRect1;
	D2D1_RECT_F crossRect2;
	D2D1_RECT_F crossRect3;
	int cursorX, cursorY;
};