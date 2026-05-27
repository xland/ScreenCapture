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
	virtual bool mouseMove(const int& x, const int& y) { return false; };
	virtual void mouseDrag(const int& x, const int& y) {};
	virtual bool mouseDown(const int& x, const int& y) { return false; };
	virtual bool mouseUp(const int& x, const int& y) { return false; };
public:
	WinPin* win;
	ShapeState state{ ShapeState::temp };
protected:
	float draggerSize;
	std::vector<D2D1_RECT_F> draggers;
	ComPtr<ID2D1SolidColorBrush> brushDragger;
private:
};

