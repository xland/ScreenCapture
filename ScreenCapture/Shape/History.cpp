#include "History.h"

static std::vector<Shape::Shape>* history;

std::vector<Shape::Shape>* History::Get()
{
	if (!history) {
		history = new std::vector<Shape::Shape>();
	}
	return history;
}