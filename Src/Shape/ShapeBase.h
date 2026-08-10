#pragma once
#include <include/Ling.h>
class WinPin;
class ShapeBase
{
public:
	ShapeBase(WinPin* win);
	virtual ~ShapeBase();
	virtual void paint(ID2D1DeviceContext* ctx) = 0;
	virtual void paintDragger(ID2D1DeviceContext* ctx) {};
	virtual void mouseMove(const float& x, const float& y) { };
	virtual void mouseDrag(const float& x, const float& y, const UINT_PTR& modifiers) {};
	virtual void mouseDown(const float& x, const float& y) { };
	virtual void mouseUp(const float& x, const float& y) { };
	virtual void mouseWheel(const float& x, const float& y, const short& delta) {};
	virtual void setCursor() {};
	bool isInRect(const D2D1_RECT_F& rect, const float& x, const float& y);
public:
	WinPin* win;
	bool isUndo;
	int hoverDraggerIndex{ -1 };
protected:
protected:
	float draggerSize;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brushDragger;
private:
};

