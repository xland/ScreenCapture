#include "History.h"
#include "../Painter.h"
#include "../MainWin.h"

static std::vector<Shape::Shape*> history;
//最后一个需要绘制的元素的下标，-1为没有需要绘制的元素
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

bool History::LastShapeHasDragger()
{
	auto& shape = history[lastDrawShapeIndex];
	if (shape->state == State::eraser || shape->state == State::pen)
	{  //这两个根本就没有MouseInDragger方法
		return false;
	}
	if (shape->state == State::mosaic && !((Shape::Mosaic*)shape)->isFill)
	{  //用画线的方式画马赛克
		return false;
	}
	return true;
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
	if (lastDrawShapeIndex < 0) return;
	auto& shape = history[lastDrawShapeIndex];
	if (!LastShapeHasDragger()) {
		ChangeCursor(IDC_CROSS);
		return;
	}
	if (!Painter::Get()->isDrawing) return;	
	if (shape->state == State::text) {
		ChangeCursor(IDC_IBEAM);
	}
	shape->MouseInDragger(pos.x, pos.y);
}
void History::LastShapeDragDragger(const POINT& pos)
{
	if (lastDrawShapeIndex < 0) return;
	history[lastDrawShapeIndex]->DragDragger(pos.x, pos.y);
}
void  History::Undo()
{
	//容器中一个元素也没有
	if (history.size() < 1) return;
	auto painter = Painter::Get();
	bool hasNeedDrawShape = false;
	int index = history.size() - 1;
	for (; index >=0 ; index--)
	{
		if (history[index]->needDraw) {
			history[index]->needDraw = false;
			hasNeedDrawShape = true;
			if (history[index]->state == State::mosaic) {
				//如果要取消的元素是马赛克，那么就干掉之前生成的马赛克背景大图
				painter->IsMosaicUsePen = false;
				delete painter->mosaicImage;
				painter->mosaicImage = nullptr;
			}
			break;
		}
	}
	//容器中一个需要绘制的元素也没有
	if (!hasNeedDrawShape) return;
	//重新在canvasImage上画所有需要绘制的元素
	auto context = painter->paintCtx;
	context->begin(*painter->canvasImage);
	context->clearAll();
	context->end();
	//最后一个需要绘制的元素的下标，-1为没有需要绘制的元素
	lastDrawShapeIndex = index - 1;
	if (lastDrawShapeIndex < 0) {
		//现在没有需要绘制的元素，不必渲染prepareImage
		painter->isDrawing = false;
		InvalidateRect(MainWin::Get()->hwnd, nullptr, false);
		return;
	}
	for (size_t i = 0; i < lastDrawShapeIndex; i++)
	{
		painter->isDrawing = true;
		history[i]->EndDraw();
		//painter->canvasImage->writeToFile("abc.png");
	}
	//把最后一个需要绘制的元素画到prepareImage上，方便用户修改
	painter->isDrawing = true;
	history[lastDrawShapeIndex]->Draw(-1, -1, -1, -1); 
	//painter->canvasImage->writeToFile("def.png");
	auto win = MainWin::Get();
	win->selectedToolIndex = (int)history[lastDrawShapeIndex]->state - 2;
	if (!LastShapeHasDragger()) {
		InvalidateRect(win->hwnd, nullptr, false);
		return;
	}
	history[lastDrawShapeIndex]->ShowDragger();
	win->preState = history[lastDrawShapeIndex]->state;
	win->state = State::lastPathDrag;
	InvalidateRect(win->hwnd, nullptr, false);	
}

void  History::Redo()
{
	//容器中一个元素也没有
	if (history.size() < 1) return;
	//最后一个都画上了,
	if (lastDrawShapeIndex + 1 >= history.size()) return;
	//下一个也画上了
	//if (history[lastDrawShapeIndex + 1]->needDraw) return;
	auto painter = Painter::Get();
	//当前这个shape是否还未被画到canvasImg上，如果是，则画上去
	//第一个元素被上一步之后，lastDrawShapeIndex就是-1
	if (lastDrawShapeIndex != -1 && painter->isDrawing) {
		history[lastDrawShapeIndex]->EndDraw();
	}	
	//得到下一个shape
	lastDrawShapeIndex += 1;
	auto& shape = history[lastDrawShapeIndex];	
	painter->isDrawing = true;
	shape->needDraw = true;
	shape->Draw(-1, -1, -1, -1);
	auto win = MainWin::Get();
	win->selectedToolIndex = (int)shape->state - 2;
	if (!LastShapeHasDragger()) {
		InvalidateRect(win->hwnd, nullptr, false);
		return;
	}
	shape->ShowDragger();
	win->preState = shape->state;
	win->state = State::lastPathDrag;
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
	for (size_t i = 0; i < history.size(); i++)
	{
		if (!history[i]->needDraw) {
			result.second = true;
			break;
		}
	}
	return result;
}
