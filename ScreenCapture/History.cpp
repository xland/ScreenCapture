#include "History.h"
#include "MainWin.h"

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
	if (shape->state == State::pen)
	{  //这两个根本就没有MouseInDragger方法
		return false;
	}
	if (shape->state == State::eraser && !((Shape::Eraser*)shape)->IsFill) {
		//用画线的方式画橡皮擦
		return false;
	}
	if (shape->state == State::mosaic && !((Shape::Mosaic*)shape)->IsFill)
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

void History::Clear() {
	auto end = std::remove_if(history.begin(),
		history.end(),
		[](Shape::Shape* item) {
			delete item;
			return true;
		});
	history.erase(end, history.end());
	lastDrawShapeIndex = -1;
}

bool History::LastShapeDrawEnd()
{
	if (lastDrawShapeIndex < 0) return true;
	return history[lastDrawShapeIndex]->EndDraw();
}
void History::LastShapeDraw(const POINT& pos1, const POINT& pos2, const State& state)
{
	if (history.size() < 1) return;
	auto shape = history[history.size() - 1];
	if (shape->state != state) return;
	shape->Draw(pos1.x, pos1.y, pos2.x, pos2.y);
}

void History::LastShapeShowDragger()
{
	auto win = WindowBase::Get();
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
	auto win = WindowBase::Get();
	if (!win->IsDrawing && shape->state != State::eraser) return;	
	if (shape->state == State::text) {
		ChangeCursor(IDC_IBEAM);
	}
	shape->MouseInDragger(pos.x, pos.y);
}
void History::LastShapeDragDragger(const int& x, const int& y)
{
	if (lastDrawShapeIndex < 0) return;
	history[lastDrawShapeIndex]->DragDragger(x, y);
}
void  History::Undo()
{
	//容器中一个元素也没有
	if (history.size() < 1) return;
	auto win = WindowBase::Get();
	bool hasNeedDrawShape = false;
	int index = history.size() - 1;
	for (; index >=0 ; index--)
	{
		if (history[index]->needDraw) {
			history[index]->needDraw = false;
			hasNeedDrawShape = true;
			if (history[index]->state == State::mosaic) {
				//如果要取消的元素是马赛克，那么就干掉之前生成的马赛克背景大图
				win->IsMosaicUsePen = false;
				delete win->MosaicImage;
				win->MosaicImage = nullptr;
			}
			else if (history[index]->state == State::eraser) {
				auto eraser = (Shape::Eraser*)history[index];
				if (eraser->canvasImgCopy) {
					delete eraser->canvasImgCopy;
					eraser->canvasImgCopy = nullptr;
				}
			}
			break;
		}
	}
	//容器中一个需要绘制的元素也没有
	if (!hasNeedDrawShape) {
		return;
	}
	//重新在CanvasImage上画所有需要绘制的元素
	auto context = win->PaintCtx;
	context->begin(*win->CanvasImage);
	context->clearAll();
	context->end();
	//最后一个需要绘制的元素的下标，-1为没有需要绘制的元素
	lastDrawShapeIndex = index - 1;
	if (lastDrawShapeIndex < 0) {
		//现在没有需要绘制的元素，不必渲染PrepareImage
		win->IsDrawing = false;
		win->state = State::maskReady;
		win->SelectedToolIndex = -1;
		win->Refresh();
		return;
	}
	for (size_t i = 0; i < lastDrawShapeIndex; i++)
	{
		win->IsDrawing = true;
		history[i]->EndDraw();
	}
	//把最后一个需要绘制的元素画到PrepareImage上，方便用户修改
	win->IsDrawing = true;
	history[lastDrawShapeIndex]->Draw(-1, -1, -1, -1); 
	
	win->SelectedToolIndex = (int)history[lastDrawShapeIndex]->state - 2;
	if (!LastShapeHasDragger()) {
		win->Refresh();
		return;
	}
	history[lastDrawShapeIndex]->ShowDragger();//todo
	win->PreState = history[lastDrawShapeIndex]->state;
	win->state = State::lastPathDrag;
	win->Refresh();	
	win->CanvasImage->writeToFile("allen.png");
}

void  History::Redo()
{
	//容器中一个元素也没有
	if (history.size() < 1) return;
	//最后一个都画上了,
	if (lastDrawShapeIndex + 1 >= history.size()) return;
	//下一个也画上了
	//if (history[lastDrawShapeIndex + 1]->needDraw) return;
	auto win = WindowBase::Get();
	//当前这个shape是否还未被画到canvasImg上，如果是，则画上去
	//第一个元素被上一步之后，lastDrawShapeIndex就是-1
	if (lastDrawShapeIndex != -1 && win->IsDrawing) {
		history[lastDrawShapeIndex]->EndDraw();
	}	
	//得到下一个shape
	lastDrawShapeIndex += 1;
	auto& shape = history[lastDrawShapeIndex];	
	win->IsDrawing = true;
	shape->needDraw = true;
	shape->Draw(-1, -1, -1, -1);
	win->SelectedToolIndex = (int)shape->state - 2;
	if (!LastShapeHasDragger()) {
		win->Refresh();
		return;
	}
	shape->ShowDragger();
	win->PreState = shape->state;
	win->state = State::lastPathDrag;
	win->Refresh();
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
