#pragma once
#include "include/core/SkCanvas.h"
#include "include/core/SkRect.h"
#include <vector>

class CutMask
{
public:
	CutMask();
	~CutMask();
	void Paint(SkCanvas* canvas);
	void PaintRect(SkCanvas* canvas);
	void PaintInfo(SkCanvas* canvas);
	void EnumWinRects();
	void onLeftBtnDown(const int& x, const int& y);
	void onLeftBtnUp(const int& x, const int& y);
	void onMouseMove(const int& x, const int& y);
	void onMouseDrag(const int& x, const int& y);
	SkRect cutRect;
private:
	SkPoint start{ -10,-10 };
	int hoverIndex{ 4 };
	std::vector<SkRect> winRects;
	void highLightWinRect(const int& x, const int& y);
	void hoverMask(const int& x, const int& y);
	void hoverBorder(const int& x, const int& y);
};

