#pragma once
#include "pch.h"
#include "ShapeBase.h"
class ShapeRect : public ShapeBase
{
public:
	ShapeRect(WinPin* win);
	~ShapeRect();
	void paint() override;
	void paintDragger() override;
	void mouseDrag(const float& x, const float& y,const UINT_PTR& modifiers) override;
	void mouseDown(const float& x, const float& y) override;
	void mouseUp(const float& x, const float& y) override;
	void mouseMove(const float& x, const float& y) override;
	void setCursor() override;
public:
private:
	std::vector<D2D1_RECT_F> draggers;
	D2D1_RECT_F rect;
	ComPtr<ID2D1SolidColorBrush> brush;
	float strokeWidth, pressX, pressY;
	bool isFill{ false };
};

