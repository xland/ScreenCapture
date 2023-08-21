#include "MainWin.h"

void MainWin::setToolBoxMain()
{
	toolBoxMain.x0 = cutBox.x1 - toolBoxWidth;
	auto heightSpan = toolBoxSpan * 3 + toolBoxHeight * 2;
	if (int(h - cutBox.y1) > heightSpan)
	{
		//屏幕底部还有足够的空间
		//两个高度，为屏幕底边也留一点间隙 
		toolBoxMain.y0 = cutBox.y1 + toolBoxSpan;
	}
	else if(int(cutBox.y0) > heightSpan)
	{
		//屏幕顶部还有足够的空间
		if (SelectedToolIndex == -1) 
		{
			//尚未确定state，主工具条贴着截图区
			toolBoxMain.y0 = cutBox.y0 - toolBoxSpan - toolBoxHeight;			
		}
		else
		{
			//已经确定了state，要为子工具条留出区域
			toolBoxMain.y0 = cutBox.y0 - (double)toolBoxSpan * 2 - (double)toolBoxHeight * 2;
		}		
	}
	else
	{
		//顶部底部都没有足够的空间
		if (SelectedToolIndex == -1) 
		{
			//尚未确定state，主工具条贴着截图区底部上方
			toolBoxMain.y0 = cutBox.y1 - toolBoxSpan - toolBoxHeight;
		}
		else
		{
			//尚未确定state，主工具条贴着截图区底部上方，并为子工具条留出空间
			toolBoxMain.y0 = cutBox.y1 - (double)toolBoxSpan * 2 - (double)toolBoxHeight * 2;
		}
	}
	if (toolBoxMain.x0 < 0) {
		//如果工具条超出了屏幕左侧，那么让工具条与截图区域左侧对齐
		//todo 如果工具条在右侧屏幕的左边缘，并超出了右侧屏幕的左边缘
		toolBoxMain.x0 = cutBox.x0;
	}
	toolBoxMain.y1 = toolBoxMain.y0 + toolBoxHeight;
	toolBoxMain.x1 = toolBoxMain.x0 + toolBoxWidth;

}

void MainWin::drawToolMain()
{
	if (state == State::start) return;
	setToolBoxMain();
	PaintCtx->setFillStyle(BLRgba32(255, 255, 255));
	PaintCtx->fillBox(toolBoxMain);
	BLPoint point;
	point.x = toolBoxMain.x0 + iconLeftMargin;
	point.y = toolBoxMain.y0 + iconTopMargin;
	for (size_t i = 0; i < 9; i++)
	{
		drawBtnCheckable(point, (Icon::Name)i, SelectedToolIndex == i, mouseEnterMainToolIndex == i);
		point.x += toolBtnWidth;
	}
	drawSplitter(point);
	point.x += toolBtnSpanWidth;
	if (IsLeftBtnDown) {
		drawBtnUndoRedo(point, Icon::Name::undo, (int)Icon::Name::undo == mouseEnterMainToolIndex, false);
		point.x += toolBtnWidth;
		drawBtnUndoRedo(point, Icon::Name::redo, (int)Icon::Name::redo == mouseEnterMainToolIndex, false);
		point.x += toolBtnWidth;
	}
	else
	{
		auto enables = History::UndoRedoEnable();
		drawBtnUndoRedo(point, Icon::Name::undo, (int)Icon::Name::undo == mouseEnterMainToolIndex, enables.first);
		point.x += toolBtnWidth;
		drawBtnUndoRedo(point, Icon::Name::redo, (int)Icon::Name::redo == mouseEnterMainToolIndex, enables.second);
		point.x += toolBtnWidth;
	}
	drawSplitter(point);
	point.x += toolBtnSpanWidth;
	//drawBtn(point, Icon::Name::pin, (int)Icon::Name::pin == mouseEnterMainToolIndex);
	//point.x += toolBtnWidth;
	drawBtn(point, Icon::Name::save, (int)Icon::Name::save == mouseEnterMainToolIndex);
	point.x += toolBtnWidth;
	drawBtn(point, Icon::Name::copy, (int)Icon::Name::copy == mouseEnterMainToolIndex);
	point.x += toolBtnWidth;
	drawBtn(point, Icon::Name::close, (int)Icon::Name::close == mouseEnterMainToolIndex);
	point.x += toolBtnWidth;
	if (SelectedToolIndex != -1) {
		drawSubTool();
	}
}

