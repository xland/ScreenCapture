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
bool History::LastShapeDrawEnd()
{
	if (history->size() < 1) return true;
	auto shape = history->at(history->size() - 1);
	if (shape->isTemp) {
		return shape->EndDraw();
	}
	else
	{
		return true;
	}
	
}
void History::LastShapeDraw(const POINT& pos1, const POINT& pos2)
{
	if (history->size() < 1) return;
	auto shape = history->at(history->size() - 1);
	shape->Draw(pos1.x, pos1.y, pos2.x, pos2.y);
}

void History::LastShapeShowDragger()
{
	if (history->size() < 1) return;
	auto shape = history->at(history->size() - 1);
	shape->ShowDragger();
}
void History::LastShapeMouseInDragger(const POINT& pos)
{
	if (history->size() < 1) return;
	auto shape = history->at(history->size() - 1);
	shape->MouseInDragger(pos.x,pos.y);
}
void History::LastShapeDragDragger(const POINT& pos)
{
	if (history->size() < 1) return;
	auto shape = history->at(history->size() - 1);
	shape->DragDragger(pos.x, pos.y);
}
