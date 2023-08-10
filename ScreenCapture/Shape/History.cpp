#include "History.h"
#include "../Painter.h"
#include "../MainWin.h"

static std::vector<Shape::Shape*> history;
static int lastDrawShapeIndex = -1;

std::vector<Shape::Shape*> History::Get()
{
	return history;
}
Shape::Shape* History::GetLastDrawShape()
{
	if (lastDrawShapeIndex < 0) return nullptr;
	return history[lastDrawShapeIndex];
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
	if (lastDrawShapeIndex < 0) return true;
	return history[lastDrawShapeIndex]->EndDraw();
}
void History::LastShapeDraw(const POINT& pos1, const POINT& pos2)
{
	if (history.size() < 1) return;
	auto shape = history[history.size() - 1];
	shape->Draw(pos1.x, pos1.y, pos2.x, pos2.y);
}

void History::LastShapeShowDragger()
{
	auto win = MainWin::Get();
	if (win->state != State::lastPathDrag) {
		lastDrawShapeIndex += 1;
		history[lastDrawShapeIndex]->isTemp = false;
	}	
	history[lastDrawShapeIndex]->ShowDragger();
}
void History::LastShapeMouseInDragger(const POINT& pos)
{
	if (!Painter::Get()->isDrawing) return;
	if (lastDrawShapeIndex < 0) return;
	if (history[lastDrawShapeIndex]->state == State::text) {
		ChangeCursor(IDC_IBEAM);
	}
	history[lastDrawShapeIndex]->MouseInDragger(pos.x, pos.y);
}
void History::LastShapeDragDragger(const POINT& pos)
{
	if (lastDrawShapeIndex < 0) return;
	history[lastDrawShapeIndex]->DragDragger(pos.x, pos.y);
}
void  History::Undo()
{
	if (history.size() < 1) return;
	bool hasNeedDrawShape = false;
	int index = history.size() - 1;
	for (; index >=0 ; index--)
	{
		if (history[index]->needDraw) {
			history[index]->needDraw = false;
			hasNeedDrawShape = true;
			break;
		}
	}
	if (!hasNeedDrawShape) return;
	auto painter = Painter::Get();
	auto context = painter->paintCtx;
	context->begin(*painter->canvasImage);
	context->clearAll();
	context->end();
	lastDrawShapeIndex = index - 1;
	if (lastDrawShapeIndex < 0) {
		painter->isDrawing = false;
		InvalidateRect(MainWin::Get()->hwnd, nullptr, false);
		return;
	}
	for (size_t i = 0; i < lastDrawShapeIndex; i++)
	{
		painter->isDrawing = true;
		history[i]->EndDraw();
	}
	painter->isDrawing = true;
	history[lastDrawShapeIndex]->Draw(-1, -1, -1, -1);
	auto win = MainWin::Get();
	if (history[lastDrawShapeIndex]->state == State::text) {
		SetTimer(win->hwnd, 999, 660, (TIMERPROC)NULL);
	}
	else
	{
		history[lastDrawShapeIndex]->ShowDragger();
	}	
	win->preState = history[lastDrawShapeIndex]->state;
	win->state = State::lastPathDrag;
	win->selectedToolIndex = (int)win->preState - 2;
	InvalidateRect(win->hwnd, nullptr, false);	
}

void  History::Redo()
{
	if (history.size() < 1 || 
		lastDrawShapeIndex + 1 >= history.size() || 
		history[lastDrawShapeIndex + 1]->needDraw) 
	{
		return;
	}
	auto painter = Painter::Get();	
	if (painter->isDrawing) {
		history[lastDrawShapeIndex]->EndDraw();
	}
	lastDrawShapeIndex += 1;
	painter->isDrawing = true;
	history[lastDrawShapeIndex]->needDraw = true;
	history[lastDrawShapeIndex]->Draw(-1, -1, -1, -1);
	auto win = MainWin::Get();
	if (history[lastDrawShapeIndex]->state == State::text) {
		SetTimer(win->hwnd, 999, 660, (TIMERPROC)NULL);
	}
	else
	{
		history[lastDrawShapeIndex]->ShowDragger();
	}	
	win->preState = history[lastDrawShapeIndex]->state;
	win->state = State::lastPathDrag;
	win->selectedToolIndex = (int)win->preState - 2;
	InvalidateRect(win->hwnd, nullptr, false);
}

std::pair<bool, bool> History::UndoRedoEnable() {
	std::pair<bool, bool> result{false,false};
	if (history.size() < 1) {
		return result;
	}
	if (history[0]->needDraw) {
		result.first = true;
	}
	if (lastDrawShapeIndex + 1 < history.size()) {
		result.second = true;
	}
	return result;
}
