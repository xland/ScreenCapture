#pragma once
#include <vector>
#include "Shape.h"

class History
{
public:
	History() = delete;
	static std::vector<Shape::Shape*>* Get();
	static void Push(Shape::Shape* shape);
	static void SaveLast();
	static void LastShapeDraw(const POINT& pos1, const POINT& pos2);
private:

};

