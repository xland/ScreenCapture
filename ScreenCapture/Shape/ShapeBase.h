#pragma once
#include "pch.h"
class WinPin;
class ShapeBase
{
public:
	ShapeBase(WinPin* win);
	virtual ~ShapeBase();
	virtual void paint() = 0;
	virtual void paintDragger() {};
	virtual void mouseMove(const float& x, const float& y) { };
	virtual void mouseDrag(const float& x, const float& y, const UINT_PTR& modifiers) {};
	virtual void mouseDown(const float& x, const float& y) { };
	virtual void mouseUp(const float& x, const float& y) { };
	virtual void mouseWheel(const float& x, const float& y, const short& delta) {};
	virtual void setCursor() {};
public:
	WinPin* win;
	bool isUndo;
	int hoverDraggerIndex{ -1 };
protected:
protected:
	float draggerSize;
	ComPtr<ID2D1SolidColorBrush> brushDragger;
private:
};

