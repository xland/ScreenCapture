#pragma once
#include "pch.h"
#include "ShapeBase.h"
class ShapeLine : public ShapeBase
{
public:
	ShapeLine(WinPin* win);
	~ShapeLine();
	void paint() override;
	void paintDragger() override;
	void mouseDrag(const int& x, const int& y,const UINT_PTR& modifiers) override;
	void mouseDown(const int& x, const int& y) override;
	void mouseUp(const int& x, const int& y) override;
	void mouseMove(const int& x, const int& y) override;
	void setCursor() override;
public:
private:
	void makePath();
	float pointToSegmentDistance(const D2D1_POINT_2F& p, const D2D1_POINT_2F& a, const D2D1_POINT_2F& b);
	void hitTest(const D2D1_POINT_2F& mousePos);
private:
	std::vector<D2D1_RECT_F> draggers;
	std::vector<D2D1_POINT_2F> linePoints;
	ComPtr<ID2D1PathGeometry> path;
	ComPtr<ID2D1StrokeStyle> roundStyle;
	ComPtr<ID2D1SolidColorBrush> brush;
	float strokeWidth, pressX, pressY;
	bool isTransparent{ false };
};

