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
	void mouseDrag(const float& x, const float& y,const UINT_PTR& modifiers) override;
	void mouseDown(const float& x, const float& y) override;
	void mouseUp(const float& x, const float& y) override;
	void mouseMove(const float& x, const float& y) override;
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
	float strokeWidth{ 1.f }, pressX{0.f}, pressY{ 0.f };
	bool isTransparent{ false };
};

