#pragma once
#include <vector>
#include "Shape/Shape.h"
#include <Windows.h>

class History
{
public:
	History() = delete;
	static std::vector<Shape::Shape*> Get();
	static void Push(Shape::Shape* shape);
	static bool LastShapeHasDragger();
	static bool LastShapeDrawEnd();
	static void LastShapeDraw(const POINT& pos1, const POINT& pos2,const State& state);
	static void LastShapeShowDragger();
	static void LastShapeMouseInDragger(const POINT& pos);
	static void LastShapeDragDragger(const int& x,const int& y);
	static void Clear();
	static void Undo();
	static void Redo();
	static Shape::Shape* GetLastDrawShape();
	static std::pair<bool,bool> UndoRedoEnable();
private:

};

