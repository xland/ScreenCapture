#pragma once
#include <vector>
#include "Shape.h"

class History
{
public:
	History() = delete;
	static std::vector<Shape::Shape*> Get();
	static void Push(Shape::Shape* shape);
	static bool LastShapeDrawEnd();
	static void LastShapeDraw(const POINT& pos1, const POINT& pos2);
	static void LastShapeShowDragger();
	static void LastShapeMouseInDragger(const POINT& pos);
	static void LastShapeDragDragger(const POINT& pos);
	static void Undo();
	static void Redo();
	static Shape::Shape* GetLastDrawShape();
	static std::pair<bool,bool> UndoRedoEnable();
private:

};

