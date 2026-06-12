#pragma once
#include "pch.h"

class WinBase;
class WinCutMask
{
public:
	WinCutMask(WinBase* win);
	~WinCutMask();
	bool highlight(const int& x, const int& y);
	void startMakeRect(const int& x, const int& y);
	void makeRect(const int& x, const int& y);
	void paint();
public:
	D2D1_RECT_F maskRect;
	int cursorIndex{-1};
	float strokeWidth;
private:
	void initWinRect();
	void makeLayout();
private:
	ComPtr<IDWriteTextFormat> textFormat;
	ComPtr<IDWriteTextLayout> textLayout;
	ComPtr<ID2D1SolidColorBrush> brushBg;
	ComPtr<ID2D1SolidColorBrush> brushBorder;
	ComPtr<ID2D1SolidColorBrush> brushText;
	ComPtr<IDWriteTextLayout> layout;
	D2D1_RECT_F layoutRect;
	std::vector<D2D1_RECT_F> winRect;
	int pressX, pressY;
	WinBase* win;
	float paddingTop{ 3 };
};

