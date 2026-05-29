#pragma once
#include "pch.h"
#include "ShapeBase.h"
class ShapeNumber : public ShapeBase
{
public:
	ShapeNumber(WinPin* win);
	~ShapeNumber();
	void paint() override;
	void paintDragger() override;
	void mouseDrag(const int& x, const int& y,const UINT_PTR& modifiers) override;
	void mouseDown(const int& x, const int& y) override;
	void mouseUp(const int& x, const int& y) override;
	void mouseMove(const int& x, const int& y) override;
	void setCursor() override;
public:
private:
	D2D1_POINT_2F localPoint(const float& degrees);
	D2D1_POINT_2F transformPoint(const D2D1_POINT_2F& point);
	void makePath();
	void makeTextLayout();
private:
	std::vector<D2D1_RECT_F> draggers;
	ComPtr<ID2D1PathGeometry> path;
	ComPtr<ID2D1SolidColorBrush> brush;
	ComPtr<ID2D1SolidColorBrush> brushText;
	ComPtr<IDWriteTextLayout> layoutText;
	float pressX, pressY,cx,cy,r,angle{ 270.f };
	bool isFill{ false };
	int val;
};

