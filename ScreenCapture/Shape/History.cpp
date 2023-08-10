#include "History.h"
#include "../Painter.h"
#include "../MainWin.h"

static std::vector<Shape::Shape*> history;

std::vector<Shape::Shape*> History::Get()
{
	return history;
}
void History::Push(Shape::Shape* shape)
{
	auto end = std::remove_if(history.begin(),
		history.end(),
		[](Shape::Shape* item) {
			if (item->isTemp || !item->needDraw) {
				delete item;
				return true;
			}
			return false;
		});
	history.erase(end, history.end());
	history.push_back(shape);	
}
bool History::LastShapeDrawEnd()
{
	if (history.size() < 1) return true;
	auto shape = history.at(history.size() - 1);
	if (!shape->needDraw) {
		return true;
	}
	return shape->EndDraw();
	
}
void History::LastShapeDraw(const POINT& pos1, const POINT& pos2)
{
	if (history.size() < 1) return;
	auto shape = history.at(history.size() - 1);
	shape->Draw(pos1.x, pos1.y, pos2.x, pos2.y);
}

void History::LastShapeShowDragger()
{
	if (history.size() < 1) return;
	auto shape = history.at(history.size() - 1);
	shape->ShowDragger();
}
void History::LastShapeMouseInDragger(const POINT& pos)
{
	if (!Painter::Get()->isDrawing) return;
	if (history.size() < 1) return;
	auto shape = history.at(history.size() - 1);
	shape->MouseInDragger(pos.x,pos.y);
}
void History::LastShapeDragDragger(const POINT& pos)
{
	if (history.size() < 1) return;
	auto shape = history.at(history.size() - 1);
	shape->DragDragger(pos.x, pos.y);
}
void  History::Undo()
{
	if (history.size() < 1) return;
	int index = history.size() - 1;
	for (; index >=0 ; index--)
	{
		if (!history.at(index)->needDraw) {
			continue;
		}
		else
		{
			history.at(index)->needDraw = false;
			break;
		}
	}
	auto painter = Painter::Get();
	auto context = painter->paintCtx;
	context->begin(*painter->canvasImage);
	context->clearAll();
	context->end();
	for (size_t i = 0; i < history.size(); i++)
	{
		if (history.at(i)->needDraw) {
			painter->isDrawing = true;
			history.at(i)->EndDraw();
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
	if (history.size() < 1) return;
	auto painter = Painter::Get();	
	for (size_t i = 0; i < history.size(); i++)
	{
		if (!history.at(i)->needDraw) {
			painter->isDrawing = true;
			history.at(i)->EndDraw();
			history.at(i)->needDraw = true;
			break;
		}		
	}
}

std::pair<bool, bool> History::UndoRedoEnable() {
	std::pair<bool, bool> result{false,false};
	if (history.size() < 1) {
		return result;
	}
	if (history.at(0)->needDraw) {
		result.first = true;
	}	
	for (int index = history.size() - 1; index >= 0; index--)
	{
		if (!history.at(index)->needDraw) {
			result.second = true;
			break;
		}
	}
	return result;
}
