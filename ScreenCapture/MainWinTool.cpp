#include "MainWin.h"

void MainWin::drawBtnBackground(const BLPoint& point, const BLRgba32& color)
{
	BLRect rect(point.x - iconLeftMargin / 2,
		point.y - iconTopMargin + iconLeftMargin / 2,
		toolBtnWidth - iconLeftMargin,
		toolBoxHeight - iconLeftMargin);
	BLVarCore val;
	paintCtx->getFillStyle(val);
	paintCtx->setFillStyle(color);
	paintCtx->fillRoundRect(rect, 6);
	paintCtx->setFillStyle(val);
}

void MainWin::drawBtn(const BLPoint& point, Icon::Name name,bool checked,bool hover,bool customColor)
{
	if (checked) {
		drawBtnBackground(point, BLRgba32(230, 244, 255, 255));
	}
	else if (hover) {
		drawBtnBackground(point, BLRgba32(238, 238, 238, 255));
	}
	if (!customColor) {
		if (checked) {
			paintCtx->setFillStyle(BLRgba32(9, 88, 217, 255));
		}
		else
		{
			paintCtx->setFillStyle(BLRgba32(30, 30, 30, 255));
		}
	}	
	auto fontSize = fontIcon->size();
	if (fontSize == 56.0f) {
		paintCtx->fillUtf8Text(BLPoint(point.x- fontSize / 2+14,point.y+11), *fontIcon, Icon::GetIcon(name));
	}
	else if (fontSize == 86.0f) {
		paintCtx->fillUtf8Text(BLPoint(point.x - fontSize / 2 + 14, point.y +22), *fontIcon, Icon::GetIcon(name));
	}
	else
	{
		paintCtx->fillUtf8Text(point, *fontIcon, Icon::GetIcon(name));
	}
	
}

void MainWin::drawSplitter(const BLPoint& point)
{
	paintCtx->setStrokeStyle(BLRgba32(180, 180, 180, 255));
	paintCtx->setStrokeWidth(0.6f);
	auto x = point.x - iconLeftMargin + toolBtnSpanWidth / 2;
	paintCtx->strokeLine(x, point.y - 23, x, point.y + 2);	
}

bool MainWin::checkMouseEnterToolBox(const POINT& pos)
{
	bool result = false;
	int tempIndex;
	if (toolBoxMain.contains(pos.x, pos.y)) {
		tempIndex = floor((pos.x - toolBoxMain.x0) / toolBtnWidth);
		result = true;
	}
	else
	{
		tempIndex = -1;
	}
	if (tempIndex != mouseEnterMainToolIndex) {
		mouseEnterMainToolIndex = tempIndex;
		setCursor(IDC_HAND);
		InvalidateRect(hwnd, nullptr, false);
		mouseInMaskBoxIndex = -1;
	}
	if(result) return result;

	if (toolBoxSub.contains(pos.x, pos.y)) {
		tempIndex = floor((pos.x - toolBoxMain.x0) / toolBtnWidth);
		result = true;
	}
	else
	{
		tempIndex = -1;
	}
	if (tempIndex != mouseEnterSubToolIndex) {
		mouseEnterSubToolIndex = tempIndex;
		setCursor(IDC_HAND);
		InvalidateRect(hwnd, nullptr, false);
		mouseInMaskBoxIndex = -1;
	}
	return result;
}