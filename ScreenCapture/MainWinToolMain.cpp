#include "MainWin.h"

void MainWin::setToolBoxMain()
{
	toolBoxMain.x0 = cutBox.x1 - toolBoxWidth;
	if (int(h - cutBox.y1) > toolBoxSpan * 3 + toolBoxHeight*2) //两个高度，为屏幕底边也留一点间隙 
	{
		toolBoxMain.y0 = cutBox.y1 + toolBoxSpan;
	}
	else if(int(cutBox.y0) > toolBoxSpan * 3 + toolBoxHeight*2)
	{
		if (selectedToolIndex == -1) 
		{
			toolBoxMain.y0 = cutBox.y0 - toolBoxSpan - toolBoxHeight;			
		}
		else
		{
			toolBoxMain.y0 = cutBox.y0 - (double)toolBoxSpan * 2 - (double)toolBoxHeight * 2;
		}		
	}
	else
	{
		if (selectedToolIndex == -1) 
		{
			toolBoxMain.y0 = cutBox.y1 - toolBoxSpan - toolBoxHeight;
		}
		else
		{
			toolBoxMain.y0 = cutBox.y1 - (double)toolBoxSpan * 2 - (double)toolBoxHeight * 2;
		}
	}
	toolBoxMain.y1 = toolBoxMain.y0 + toolBoxHeight;
	toolBoxMain.x1 = toolBoxMain.x0 + toolBoxWidth;
}

void MainWin::drawToolMain()
{
	if (state == State::start) return;
	if (!fontIcon) {
		initFontIcon();
	}
	setToolBoxMain();
	paintCtx->setFillStyle(BLRgba32(255, 255, 255, 255));
	paintCtx->fillBox(toolBoxMain);	
	BLPoint point;
	point.x = toolBoxMain.x0 + iconLeftMargin;
	point.y = toolBoxMain.y0 + iconTopMargin;
	for (size_t i = 0; i < 9; i++)
	{
		drawBtn(point, (Icon::Name)i, selectedToolIndex == i, mouseEnterMainToolIndex == i);
		point.x += toolBtnWidth;
	}
	drawSplitter(point);
	point.x += toolBtnSpanWidth;
	drawBtn(point, Icon::Name::undo, false,(int)Icon::Name::undo == mouseEnterMainToolIndex);
	point.x += toolBtnWidth;
	drawBtn(point, Icon::Name::redo, false, (int)Icon::Name::redo == mouseEnterMainToolIndex);
	point.x += toolBtnWidth;
	drawSplitter(point);
	point.x += toolBtnSpanWidth;
	drawBtn(point, Icon::Name::save, false, (int)Icon::Name::save == mouseEnterMainToolIndex);
	point.x += toolBtnWidth;
	drawBtn(point, Icon::Name::copy, false, (int)Icon::Name::copy == mouseEnterMainToolIndex);
	point.x += toolBtnWidth;
	drawBtn(point, Icon::Name::close, false, (int)Icon::Name::close == mouseEnterMainToolIndex);
	point.x += toolBtnWidth;
	if (selectedToolIndex != -1) {
		drawSubTool();
	}
}

