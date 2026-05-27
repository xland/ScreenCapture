#pragma once
#include "pch.h"

enum class ShapeState
{
	temp = 0,
	ready,
	undo,
	sizing0,
	sizing1,
	sizing2,
	sizing3,
	sizing4,
	sizing5,
	sizing6,
	sizing7,
	moving,
};
class WinPin;
class ShapeBase
{
public:
	ShapeBase(WinPin* win);
	~ShapeBase();
	virtual void paint() = 0;
	virtual void paintDragger() {};
	virtual void mouseMove(const int& x, const int& y) { };
	virtual void mouseDrag(const int& x, const int& y) {};
	virtual void mouseDown(const int& x, const int& y) { };
	virtual void mouseUp(const int& x, const int& y) { };
	virtual void setCursor() {};
public:
	WinPin* win;
	ShapeState state{ ShapeState::temp };
	int hoverDraggerIndex{ -1 };
protected:
	float draggerSize;
	ComPtr<ID2D1SolidColorBrush> brushDragger;
private:
};

