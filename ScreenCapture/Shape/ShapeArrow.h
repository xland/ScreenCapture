#pragma once
#include "ShapeBase.h"
class ShapeArrow : public ShapeBase
{
public:
	ShapeArrow(WinPin* win);
	~ShapeArrow();
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
	ComPtr<ID2D1PathGeometry> path;
	ComPtr<ID2D1SolidColorBrush> brush;
	float strokeWidth, pressX, pressY,startX,startY,endX,endY;
	bool isFill{ false };
};

