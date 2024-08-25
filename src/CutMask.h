#pragma once
#include "include/core/SkCanvas.h"
#include "include/core/SkRect.h"

class CutMask
{
public:
	CutMask();
	~CutMask();
	void Paint(SkCanvas* canvas);
	bool onLeftBtnDown(const int& x, const int& y);
	bool onLeftBtnUp(const int& x, const int& y);
	bool onMouseMove(const int& x, const int& y);
	bool onMouseDrag(const int& x, const int& y);
	SkRect cutRect;
private:
	SkPoint start{ -10,-10 };
	int hoverIndex{ 4 };
	std::vector<SkRect> winRects;
};

