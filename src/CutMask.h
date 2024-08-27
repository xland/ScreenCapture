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
	void OnPaint(SkCanvas* canvas);
	void OnLeftBtnDown(const int& x, const int& y);
	void OnLeftBtnUp(const int& x, const int& y);
	void OnMouseMove(const int& x, const int& y);
	void OnMouseDrag(const int& x, const int& y);
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

