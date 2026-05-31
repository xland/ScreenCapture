#pragma once
#include "pch.h"
class ShapeBase;
class WinPin;
class History
{
public:
	History(WinPin* win);
	~History();
	ShapeBase* createShape(const std::string& state, const int& x, const int& y);
	void undo();
	void redo();
public:
	std::vector<std::unique_ptr<ShapeBase>> shapes;
private:
	void removeUndoShape();
private:
	WinPin* win;
};

