#include "History.h"
#include "../Painter.h"
#include "../MainWin.h"

static std::vector<Shape::Shape*>* history{new std::vector<Shape::Shape*>()};

std::vector<Shape::Shape*>* History::Get()
{
	return history;
}
void History::Push(Shape::Shape* shape)
{
	auto end = std::remove_if(history->begin(),
		history->end(),
		[](Shape::Shape* item) {
			if (item->isTemp || !item->needDraw) {
				delete item;
				return true;
			}
			return false;
		});
	history->erase(end, history->end());
/*	int index = -1;
	int endIndex = history->size() - 1;
	for (int i = 0; i < history->size(); i++)
	{
		auto& shape = history->at(i);
		if (shape->isTemp || !shape->needDraw) {
			index = i;
			break;
		}
	}
	if (index != -1) {
		for (int i = index; i <= endIndex; i++)
		{
			delete history->at(i);
		}
		history->erase(history->begin() + index,history->begin()+endIndex);
	}*/	
	history->push_back(shape);	
}
bool History::LastShapeDrawEnd()
{
	if (history->size() < 1) return true;
	auto shape = history->at(history->size() - 1);
	return shape->EndDraw();
	
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
void  History::Undo()
{
	if (history->size() < 1) return;
	int index = history->size() - 1;
	for (; index >=0 ; index--)
	{
		if (!history->at(index)->needDraw) {
			continue;
		}
		else
		{
			history->at(index)->needDraw = false;
			break;
		}
	}
	auto painter = Painter::Get();
	auto context = painter->paintCtx;
	context->begin(*painter->canvasImage);
	context->clearAll();
	context->end();
	for (size_t i = 0; i < history->size(); i++)
	{
		if (history->at(i)->needDraw) {
			painter->isDrawing = true;
			history->at(i)->EndDraw();
		}
		else
		{
			break;
		}		
	}
	auto win = MainWin::Get();
	InvalidateRect(win->hwnd, nullptr, false);
}

void  History::Redo()
{
	if (history->size() < 1) return;
	auto painter = Painter::Get();	
	for (size_t i = 0; i < history->size(); i++)
	{
		if (!history->at(i)->needDraw) {
			painter->isDrawing = true;
			history->at(i)->EndDraw();
			history->at(i)->needDraw = true;
			break;
		}		
	}
}
