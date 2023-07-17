#include "MainWin.h"

static int btnCount = 14;
static int span = 12;//工具条距离截图区域的高度
static int toolBtnSpanWidth = 6;
static int toolBtnSpanCount = 2;
static int toolBtnWidth = 60;
static int toolBoxWidth = btnCount * toolBtnWidth + toolBtnSpanWidth * toolBtnSpanCount;
static int toolBoxHeight = 56;
static int iconLeftSpan = 16;



void MainWin::setToolBox()
{
	toolBox.x0 = cutBox.x1 - toolBoxWidth;
	if (h - cutBox.y1 > span * 3 + toolBoxHeight*2) //两个高度，为屏幕底边也留一点间隙 
	{
		toolBox.y0 = cutBox.y1 + span;
	}
	else if(cutBox.y0 > span * 3 + toolBoxHeight*2)
	{
		if (selectedToolIndex == -1) 
		{
			toolBox.y0 = cutBox.y0 - span - toolBoxHeight;			
		}
		else
		{
			toolBox.y0 = cutBox.y0 - span * 2 - toolBoxHeight * 2;
		}		
	}
	else
	{
		if (selectedToolIndex == -1) 
		{
			toolBox.y0 = cutBox.y1 - span - toolBoxHeight;
		}
		else
		{
			toolBox.y0 = cutBox.y1 - span * 2 - toolBoxHeight * 2;
		}
	}
	toolBox.y1 = toolBox.y0 + toolBoxHeight;
	toolBox.x1 = toolBox.x0 + toolBoxWidth;
}

void MainWin::checkMouseEnterToolBox(const POINT& pos)
{
	int tempIndex;
	if (toolBox.contains(pos.x, pos.y)) {
		tempIndex = floor((pos.x - toolBox.x0) / toolBtnWidth);
	}
	else
	{
		tempIndex = -1;
	}
	if (tempIndex != mouseEnterToolIndex) {
		mouseEnterToolIndex = tempIndex;
		InvalidateRect(hwnd, nullptr, false);
	}
}

void MainWin::drawSplitter(BLPoint& point)
{
	paintCtx->setStrokeStyle(BLRgba32(180, 180, 180, 255));
	paintCtx->setStrokeWidth(0.6f);
	auto x = point.x - iconLeftSpan + toolBtnSpanWidth / 2;
	paintCtx->strokeLine(x, point.y - 23, x, point.y + 2);
	point.x += toolBtnSpanWidth;
}

void MainWin::drawBtn(BLPoint& point,Icon::Name name)
{
	if ((int)name == mouseEnterToolIndex) {
		paintCtx->setFillStyle(BLRgba32(220, 220, 220, 255));
		paintCtx->fillBox(BLBox(point.x - iconLeftSpan, toolBox.y0, point.x - iconLeftSpan + toolBtnWidth, toolBox.y0 + toolBoxHeight));		
	}
	paintCtx->setFillStyle(BLRgba32(30, 30, 30, 255));
	paintCtx->fillUtf8Text(point, *fontIcon, Icon::Get(name));
	point.x += toolBtnWidth;
}

void MainWin::drawTool()
{
	if (state == State::Start) return;
	if (!fontIcon) {
		initFontIcon();
	}
	setToolBox();
	paintCtx->setFillStyle(BLRgba32(240, 240, 240, 255));
	paintCtx->fillBox(toolBox);	
	BLPoint point;
	point.x = toolBox.x0 + iconLeftSpan;
	point.y = toolBox.y0 + 38;
	for (size_t i = 0; i < 9; i++)
	{
		if (selectedToolIndex == i) {
			paintCtx->setFillStyle(BLRgba32(22, 119, 255, 255));
			paintCtx->fillBox(BLBox(point.x - iconLeftSpan, toolBox.y0, point.x - iconLeftSpan + toolBtnWidth, toolBox.y0 + toolBoxHeight));
			paintCtx->setFillStyle(BLRgba32(255, 255, 255, 255));
		}
		else if (mouseEnterToolIndex == i) {
			paintCtx->setFillStyle(BLRgba32(220, 220, 220, 255));
			paintCtx->fillBox(BLBox(point.x - iconLeftSpan, toolBox.y0, point.x - iconLeftSpan + toolBtnWidth, toolBox.y0 + toolBoxHeight));
			paintCtx->setFillStyle(BLRgba32(30, 30, 30, 255));
		}
		else
		{
			paintCtx->setFillStyle(BLRgba32(30, 30, 30, 255));
		}
		paintCtx->fillUtf8Text(point, *fontIcon, Icon::Get((Icon::Name)i));
		point.x += toolBtnWidth;
	}
	drawSplitter(point);
	drawBtn(point, Icon::Name::undo);
	drawBtn(point, Icon::Name::redo);
	drawSplitter(point);
	drawBtn(point, Icon::Name::save);
	drawBtn(point, Icon::Name::copy);
	drawBtn(point, Icon::Name::close);		
	if (selectedToolIndex != -1) {
		drawSubTool();
	}
}

void MainWin::drawSubTool()
{
	paintCtx->setFillStyle(BLRgba32(240, 240, 240, 255));
	auto x = toolBox.x0 + selectedToolIndex*toolBtnWidth + toolBtnWidth / 2;
	auto y = toolBox.y1 + span / 2;
	paintCtx->fillTriangle(x,y,x+6,y+span/2,x-6,y+span/2);
}