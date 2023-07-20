#include "History.h"

static std::vector<Shape::Shape*>* history{new std::vector<Shape::Shape*>()};

std::vector<Shape::Shape*>* History::Get()
{
	return history;
}
void History::Push(Shape::Shape* shape)
{
	auto size = history->size();
	if (size > 0 && history->at(size - 1)->isTemp)
	{
		history->pop_back();
	}
	history->push_back(shape);	
}
void History::SaveLast()
{
	auto size = history->size();
	if (size > 0)
	{
		history->at(size - 1)->isTemp = false;
	}
}