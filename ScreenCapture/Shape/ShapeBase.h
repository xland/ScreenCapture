#pragma once
#include "pch.h"

class WinPin;
class ShapeBase
{
public:
	ShapeBase(WinPin* win);
	~ShapeBase();
	virtual void paint() = 0;
	virtual bool mouseMove(const int& x, const int& y) { return false; };
	virtual void mouseDrag(const int& x, const int& y) {};
	virtual bool mouseDown(const int& x, const int& y) { return false; };
	virtual bool mouseUp(const int& x, const int& y) { return false; };
public:
	WinPin* win;
protected:
private:
};

