#pragma once
#include "ShapeBase.h"
class ShapeArrow : public ShapeBase
{
public:
	ShapeArrow(WinPin* win);
	~ShapeArrow();
	void paint() override;
	void paintDragger() override;
	void mouseDrag(const float& x, const float& y, const UINT_PTR& modifiers) override;
	void mouseDown(const float& x, const float& y) override;
	void mouseUp(const float& x, const float& y) override;
	void mouseMove(const float& x, const float& y) override;
	void setCursor() override;
public:
private:
	void makeArrow();
	void constrainToEightDirections(const float& anchorX, const float& anchorY, const float& mouseX, const float& mouseY, float& targetX, float& targetY);
private:
	std::vector<D2D1_RECT_F> draggers; 
	ComPtr<ID2D1PathGeometry> path;
	ComPtr<ID2D1SolidColorBrush> brush;
	float arrowSize{ 0 }, pressX{ 0 }, pressY{ 0 }, startX{ 0 }, startY{ 0 }, endX{ 0 }, endY{ 0 };
	bool isFill{ false };
};

