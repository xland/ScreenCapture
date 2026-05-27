#pragma once
#include "pch.h"
#include "ShapeBase.h"
class WinPin;
class ShapeRect : public ShapeBase
{
public:
	ShapeRect(WinPin* win);
	~ShapeRect();
	void paint() override;
	void paintDragger() override;
	void mouseDrag(const int& x, const int& y) override;
	void mouseDown(const int& x, const int& y) override;
	void mouseUp(const int& x, const int& y) override;
	void mouseMove(const int& x, const int& y) override;
	void setCursor() override;
public:
	std::vector<D2D1_RECT_F> draggers;
	D2D1_RECT_F rect;
	ComPtr<ID2D1SolidColorBrush> brush;
	float strokeWidth,pressX,pressY;
};

