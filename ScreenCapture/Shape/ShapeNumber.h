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
	void mouseDrag(const float& x, const float& y,const UINT_PTR& modifiers) override;
	void mouseDown(const float& x, const float& y) override;
	void mouseUp(const float& x, const float& y) override;
	void mouseMove(const float& x, const float& y) override;
	void mouseWheel(const float& x, const float& y, const short& delta) override;
	void setCursor() override;
public:
private:
	static int getNextVal(WinPin* win);
	D2D1_POINT_2F localPoint(const float& degrees);
	D2D1_POINT_2F transformPoint(const D2D1_POINT_2F& point);
	void makePath();
private:
	std::vector<D2D1_RECT_F> draggers;
	ComPtr<ID2D1PathGeometry> path;
	ComPtr<ID2D1SolidColorBrush> brush;
	ComPtr<ID2D1SolidColorBrush> brushText;
	ComPtr<IDWriteTextLayout> layoutText;
	float pressX, pressY,cx,cy,r,angle{ 270.f };
	D2D1_POINT_2F tip,mid;
	bool isFill{ false }, isWheel{false};
	int val;
};

