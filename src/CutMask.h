#pragma once
#include "include/core/SkCanvas.h"
#include "include/core/SkRect.h"
#include <vector>
#include "EventHandler.h"

class CutMask:public EventHandler
{
public:
	CutMask();
	~CutMask();
	void Init();
	void onPaint(SkCanvas* canvas);
	void onLeftBtnDown(const int& x, const int& y);
	void onLeftBtnUp(const int& x, const int& y);
	void onMouseMove(const int& x, const int& y);
	void onMouseDrag(const int& x, const int& y);
	SkRect cutRect;
private:
	void paintRect(SkCanvas* canvas);
	void paintInfo(SkCanvas* canvas);
	void enumWinRects();
	SkPoint start{ -10,-10 };
	int hoverIndex{ 4 };
	std::vector<SkRect> winRects;
	void highLightWinRect(const int& x, const int& y);
	void hoverMask(const int& x, const int& y);
	void hoverBorder(const int& x, const int& y);
};

