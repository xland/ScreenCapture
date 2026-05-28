#pragma once
#include "ShapeBase.h"
class ShapeEllipse : public ShapeBase
{
public:
	ShapeEllipse(WinPin* win);
	~ShapeEllipse();
	void paint() override;
	void paintDragger() override;
	void mouseDrag(const int& x, const int& y) override;
	void mouseDown(const int& x, const int& y) override;
	void mouseUp(const int& x, const int& y) override;
	void mouseMove(const int& x, const int& y) override;
	void setCursor() override;
public:
private:
	std::vector<D2D1_RECT_F> draggers;
	D2D1_RECT_F rect;
	ComPtr<ID2D1SolidColorBrush> brush;
	float strokeWidth, pressX, pressY,cx,cy,rx,ry;
	bool isFill{ false };
};

