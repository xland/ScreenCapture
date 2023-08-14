#include "MainWin.h"


BLRect MainWin::getBtnRect(const BLPoint& point)
{
	BLRect rect(point.x - iconLeftMargin / 2,
		point.y - iconTopMargin + iconLeftMargin / 2,
		toolBtnWidth - iconLeftMargin,
		toolBoxHeight - iconLeftMargin);
	return rect;
}

void MainWin::drawBtn(const BLPoint& point, const Icon::Name& name,const bool& hover)
{
	if (hover) {
		auto rect = getBtnRect(point);
		painter->paintCtx->setFillStyle(BLRgba32(238, 238, 238));
		painter->paintCtx->fillRoundRect(rect, 6);
	}
	painter->paintCtx->setFillStyle(BLRgba32(30, 30, 30, 255));
	painter->paintCtx->fillUtf8Text(point, *Font::Get()->fontIcon, Icon::GetIcon(name));
}


void MainWin::drawBtnCheckable(const BLPoint& point, const Icon::Name& name, const bool& checked, const bool& hover)
{
	if (checked) {
		auto rect = getBtnRect(point);
		painter->paintCtx->setFillStyle(BLRgba32(230, 244, 255));
		painter->paintCtx->fillRoundRect(rect, 6);
		painter->paintCtx->setFillStyle(BLRgba32(9, 88, 217, 255));
	}
	else
	{
		if (hover) {
			auto rect = getBtnRect(point);
			painter->paintCtx->setFillStyle(BLRgba32(238, 238, 238));
			painter->paintCtx->fillRoundRect(rect, 6);
		}
		painter->paintCtx->setFillStyle(BLRgba32(30, 30, 30, 255));
	}
	painter->paintCtx->fillUtf8Text(point, *Font::Get()->fontIcon, Icon::GetIcon(name));
}


void MainWin::drawBtnStrokeWidth(const BLPoint& point, const bool& checked, const bool& hover)
{
	if (checked) {
		auto rect = getBtnRect(point);
		painter->paintCtx->setFillStyle(BLRgba32(230, 244, 255));
		painter->paintCtx->fillRoundRect(rect, 6);
		painter->paintCtx->setFillStyle(BLRgba32(9, 88, 217, 255));
	}
	else
	{
		if (hover) {
			auto rect = getBtnRect(point);
			painter->paintCtx->setFillStyle(BLRgba32(238, 238, 238));
			painter->paintCtx->fillRoundRect(rect, 6);
		}
		painter->paintCtx->setFillStyle(BLRgba32(30, 30, 30, 255));
	}
	auto iconChar = Icon::GetIcon(Icon::Name::dot);
	auto fontSize = Font::Get()->fontIcon->size();
	if (fontSize == 56.0f) {
		painter->paintCtx->fillUtf8Text(BLPoint(point.x - fontSize / 2 + 14, point.y + 11),
			*Font::Get()->fontIcon, iconChar);
	}
	else if (fontSize == 86.0f) {
		painter->paintCtx->fillUtf8Text(BLPoint(point.x - fontSize / 2 + 14, point.y + 22),
			*Font::Get()->fontIcon, iconChar);
	}
	else
	{
		painter->paintCtx->fillUtf8Text(point, *Font::Get()->fontIcon, iconChar);
	}
}

void MainWin::drawBtnColors(const BLPoint& point, const Icon::Name& name, const bool& checked, const bool& hover, const BLRgba32& color)
{

	auto rect = getBtnRect(point);
	if (checked) {
		painter->paintCtx->setFillStyle(BLRgba32(230, 244, 255));
		painter->paintCtx->fillRoundRect(rect, 6);
	}
	else if (hover) {
		painter->paintCtx->setFillStyle(BLRgba32(238, 238, 238));
		painter->paintCtx->fillRoundRect(rect, 6);
	}
	painter->paintCtx->setFillStyle(color);
	painter->paintCtx->fillUtf8Text(point, *Font::Get()->fontIcon, Icon::GetIcon(name));
}

void MainWin::drawBtnUndoRedo(const BLPoint& point, const Icon::Name& name, const bool& hover, const bool& enable)
{
	if (!enable) {
		painter->paintCtx->setFillStyle(BLRgba32(180, 180, 180, 255));
	}
	else {
		if (hover) {
			auto rect = getBtnRect(point);
			painter->paintCtx->setFillStyle(BLRgba32(238, 238, 238));
			painter->paintCtx->fillRoundRect(rect, 6);
		}
		painter->paintCtx->setFillStyle(BLRgba32(30, 30, 30, 255));
	}
	painter->paintCtx->fillUtf8Text(point, *Font::Get()->fontIcon, Icon::GetIcon(name));
}


void MainWin::drawSplitter(const BLPoint& point)
{
	painter->paintCtx->setStrokeStyle(BLRgba32(180, 180, 180, 255));
	painter->paintCtx->setStrokeWidth(0.6f);
	auto x = point.x - iconLeftMargin + toolBtnSpanWidth / 2;
	painter->paintCtx->strokeLine(x, point.y - 23, x, point.y + 2);
}

bool MainWin::checkMouseEnterToolBox(const POINT& pos)
{
	bool result = false;
	int tempIndex;
	if (toolBoxMain.contains(pos.x, pos.y)) {
		tempIndex = floor((pos.x - toolBoxMain.x0) / toolBtnWidth);
		result = true;
		ChangeCursor(IDC_HAND);
	}
	else
	{
		tempIndex = -1;
	}
	if (tempIndex != mouseEnterMainToolIndex) {
		mouseEnterMainToolIndex = tempIndex;		
		InvalidateRect(hwnd, nullptr, false);
		mouseInMaskBoxIndex = -1;
	}
	if(result) return result;

	if (toolBoxSub.contains(pos.x, pos.y)) {
		tempIndex = floor((pos.x - toolBoxSub.x0) / toolBtnWidth);
		ChangeCursor(IDC_HAND);
		result = true;
	}
	else
	{
		tempIndex = -1;
	}
	if (tempIndex != mouseEnterSubToolIndex) {
		mouseEnterSubToolIndex = tempIndex;
		InvalidateRect(hwnd, nullptr, false);
		mouseInMaskBoxIndex = -1;
	}
	return result;
}