#pragma once
#include "pch.h"
#include "WinBase.h"

class WinStart:public WinBase
{
public:
	WinStart(const int& x, const int& y, const int& w, const int& h);
	~WinStart();
public:
private:
	void onPaint() override;
	void onMouseMove(const int& x, const int& y) override;
	void onMouseDrag(const int& x, const int& y, const UINT_PTR& modifiers) override;
	void onMouseDown(const int& x, const int& y, bool isRight) override;
	void onMouseUp(const int& x, const int& y) override;
	BOOL onCursor() override;
private:
	ComPtr<ID2D1SolidColorBrush> textBrush;
	ComPtr<ID2D1SolidColorBrush> bgBrush;
	ComPtr<IDWriteTextLayout> layoutText;
};

