#include "WindowBase.h"


BLRoundRect WindowBase::getBtnRect(const BLPoint& point)
{
	return BLRoundRect(point.x - iconLeftMargin / 2, point.y - iconTopMargin + iconLeftMargin / 2,
		toolBtnWidth - iconLeftMargin, toolBoxHeight - iconLeftMargin,6);
}

void WindowBase::drawBtn(const BLPoint& point, const Icon::Name& name,const bool& hover)
{
	if (hover) {
		auto rect = getBtnRect(point);
		PaintCtx->setFillStyle(BLRgba32(238, 238, 238));
		PaintCtx->fillRoundRect(rect);
	}
	PaintCtx->setFillStyle(BLRgba32(30, 30, 30));
	PaintCtx->fillUtf8Text(point, *Font::Get()->fontIcon, Icon::GetIcon(name));
}


void WindowBase::drawBtnCheckable(const BLPoint& point, const Icon::Name& name, const bool& checked, const bool& hover)
{
	if (checked) {
		auto rect = getBtnRect(point);
		PaintCtx->setFillStyle(BLRgba32(230, 244, 255));
		PaintCtx->fillRoundRect(rect);
		PaintCtx->setFillStyle(BLRgba32(9, 88, 217));
	}
	else
	{
		if (hover) {
			auto rect = getBtnRect(point);
			PaintCtx->setFillStyle(BLRgba32(238, 238, 238));
			PaintCtx->fillRoundRect(rect);
		}
		PaintCtx->setFillStyle(BLRgba32(30, 30, 30));
	}
	PaintCtx->fillUtf8Text(point, *Font::Get()->fontIcon, Icon::GetIcon(name));
}


void WindowBase::drawBtnStrokeWidth(const BLPoint& point, const bool& checked, const bool& hover)
{
	if (checked) {
		auto rect = getBtnRect(point);
		PaintCtx->setFillStyle(BLRgba32(230, 244, 255));
		PaintCtx->fillRoundRect(rect);
		PaintCtx->setFillStyle(BLRgba32(9, 88, 217));
	}
	else
	{
		if (hover) {
			auto rect = getBtnRect(point);
			PaintCtx->setFillStyle(BLRgba32(238, 238, 238));
			PaintCtx->fillRoundRect(rect);
		}
		PaintCtx->setFillStyle(BLRgba32(30, 30, 30));
	}
	auto iconChar = Icon::GetIcon(Icon::Name::dot);
	auto fontSize = Font::Get()->fontIcon->size();
	if (fontSize == 56.0f) {
		PaintCtx->fillUtf8Text(BLPoint(point.x - fontSize / 2 + 14, point.y + 11),
			*Font::Get()->fontIcon, iconChar);
	}
	else if (fontSize == 86.0f) {
		PaintCtx->fillUtf8Text(BLPoint(point.x - fontSize / 2 + 14, point.y + 22),
			*Font::Get()->fontIcon, iconChar);
	}
	else
	{
		PaintCtx->fillUtf8Text(point, *Font::Get()->fontIcon, iconChar);
	}
}

void WindowBase::drawBtnColors(const BLPoint& point, const Icon::Name& name, const bool& checked, const bool& hover, const BLRgba32& color)
{

	auto rect = getBtnRect(point);
	if (checked) {
		PaintCtx->setFillStyle(BLRgba32(230, 244, 255));
		PaintCtx->fillRoundRect(rect);
	}
	else if (hover) {
		PaintCtx->setFillStyle(BLRgba32(238, 238, 238));
		PaintCtx->fillRoundRect(rect);
	}
	PaintCtx->setFillStyle(color);
	PaintCtx->fillUtf8Text(point, *Font::Get()->fontIcon, Icon::GetIcon(name));
}

void WindowBase::drawBtnUndoRedo(const BLPoint& point, const Icon::Name& name, const bool& hover, const bool& enable)
{
	if (!enable) {
		PaintCtx->setFillStyle(BLRgba32(180, 180, 180));
	}
	else {
		if (hover) {
			auto rect = getBtnRect(point);
			PaintCtx->setFillStyle(BLRgba32(238, 238, 238));
			PaintCtx->fillRoundRect(rect);
		}
		PaintCtx->setFillStyle(BLRgba32(30, 30, 30));
	}
	PaintCtx->fillUtf8Text(point, *Font::Get()->fontIcon, Icon::GetIcon(name));
}


void WindowBase::drawSplitter(const BLPoint& point)
{
	PaintCtx->setStrokeStyle(BLRgba32(180, 180, 180));
	PaintCtx->setStrokeWidth(0.6f);
	auto x = point.x - iconLeftMargin + toolBtnSpanWidth / 2;
	PaintCtx->strokeLine(x, point.y - 23, x, point.y + 2);
}

bool WindowBase::checkMouseEnterToolBox(const POINT& pos)
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
		Refresh();
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
		Refresh();
		mouseInMaskBoxIndex = -1;
	}
	return result;
}