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

bool WindowBase::checkMouseEnterToolBox(const int& x, const int& y)
{
	bool result = false;
	int tempIndex;
	if (toolBoxMain.contains(x, y)) {
		tempIndex = floor((x - toolBoxMain.x0) / toolBtnWidth);
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

	if (toolBoxSub.contains(x, y)) {
		tempIndex = floor((x - toolBoxSub.x0) / toolBtnWidth);
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

void WindowBase::drawToolMain()
{	
	PaintCtx->setFillStyle(BLRgba32(255, 255, 255));
	PaintCtx->fillBox(toolBoxMain);
	PaintCtx->setStrokeWidth(1.0f);
	PaintCtx->strokeBox(toolBoxMain, BLRgba32(9, 88, 217));
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
	drawBtn(point, Icon::Name::pin, (int)Icon::Name::pin == mouseEnterMainToolIndex);
	point.x += toolBtnWidth;
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