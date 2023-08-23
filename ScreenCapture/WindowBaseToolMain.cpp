#include "WindowBase.h"



void WindowBase::drawToolMain()
{
	if (state == State::start) return;
	SetToolMainPos();
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

