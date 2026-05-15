#pragma once
#include "pch.h"

class CutMask
{
public:
	CutMask();
	~CutMask();
	static void init();
	static CutMask* get();
	bool highlight(const int& x, const int& y);
	void changeCursor(const int& x, const int& y);
	void startMakeRect(const int& x, const int& y);
	void startChangeRect(const int& x, const int& y);
	void changeRect(const int& x, const int& y);
	void makeRect(const int& x, const int& y);
	void paint();
public:
	D2D1_RECT_F maskRect;
	D2D1_RECT_F originalRect;
	int cursorIndex{-1};
private:
	void initWinRect();
private:
	winrt::com_ptr<IDWriteTextFormat> textFormat;
	winrt::com_ptr<IDWriteTextLayout> textLayout;
	winrt::com_ptr<ID2D1SolidColorBrush> brushBg;
	winrt::com_ptr<ID2D1SolidColorBrush> brushBorder;
	winrt::com_ptr<ID2D1SolidColorBrush> brushText;
	std::vector<D2D1_RECT_F> winRect;
	int pressX, pressY;
};

