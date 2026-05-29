#pragma once
#include "pch.h"
class WinPin;
class ShapeBase
{
public:
	ShapeBase(WinPin* win);
	~ShapeBase();
	virtual void paint() = 0;
	virtual void paintDragger() {};
	virtual void mouseMove(const int& x, const int& y) { };
	virtual void mouseDrag(const int& x, const int& y, const UINT_PTR& modifiers) {};
	virtual void mouseDown(const int& x, const int& y) { };
	virtual void mouseUp(const int& x, const int& y) { };
	virtual void setCursor() {};
public:
	WinPin* win;
	bool isUndo;
	int hoverDraggerIndex{ -1 };
protected:
	float draggerSize;
	ComPtr<ID2D1SolidColorBrush> brushDragger;
private:
};

