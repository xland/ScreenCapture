#include "pch.h"
#include "../Win/WinPin.h"
#include "../Win/WinToolMain.h"
#include "../Win/WinToolSub.h"
#include "../Util.h"
#include "ShapeText.h"
#include "ShapeTextWin.h"

ShapeText::ShapeText(WinPin* win) :ShapeBase(win), borderPadding{ 6.f * win->dpi }, draggers{
	D2D1::RectF(0,0,0,0),
	D2D1::RectF(0,0,0,0),
	D2D1::RectF(0,0,0,0),
	D2D1::RectF(0,0,0,0) }
{
	auto toolSub = WinToolSub::get();
	color = toolSub->getSelectedColor();
	fontSize = toolSub->sliderVal;
	isBold = toolSub->selectIndex == 0;
	isItalic = toolSub->selectIndex == 1;
	float dashes[] = { 2.0f, 2.0f };
	win->getD2D()->CreateStrokeStyle(
		D2D1::StrokeStyleProperties(
			D2D1_CAP_STYLE_FLAT,      // 起始端点样式
			D2D1_CAP_STYLE_FLAT,      // 结束端点样式
			D2D1_CAP_STYLE_ROUND,     // 拐角处线帽（对矩形无影响）
			D2D1_LINE_JOIN_MITER,     // 线段连接方式
			10.0f,                    // Miter limit（斜接限制）
			D2D1_DASH_STYLE_CUSTOM,   // 自定义虚线样式（关键！）
			0.0f                      // 虚线起始偏移
		),
		dashes,                       // 虚线数组
		ARRAYSIZE(dashes),            // 数组长度
		&dashedStrokeStyle
	);
}

ShapeText::~ShapeText()
{

}

void ShapeText::paint()
{
	if (isEditing|| hoverDraggerIndex==1|| hoverDraggerIndex==8) {
		win->render->DrawRectangle(rect, textBrush.Get(), win->dpi, dashedStrokeStyle.Get());
	}
	if(!isEditing)
	{
		win->render->DrawTextLayout({ (float)rect.left+borderPadding, (float)rect.top+borderPadding }, textLayout.Get(), textBrush.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
	}
}

void ShapeText::paintDragger()
{
	for (auto& dragger : draggers)
	{
		win->render->DrawRectangle(dragger, brushDragger.Get(), win->dpi);
	}
}

void ShapeText::mouseDrag(const float& x, const float& y, const UINT_PTR& modifiers)
{
	if (hoverDraggerIndex == 8)
	{
		auto spanX{ x - pressX };
		auto spanY{ y - pressY };
		rect.left += spanX;
		rect.right += spanX;
		rect.top += spanY;
		rect.bottom += spanY;
		pressX = x;
		pressY = y;
	}
}

void ShapeText::mouseDown(const float& x, const float& y)
{
	pressX = x;
	pressY = y;
	if (hoverDraggerIndex == -1) { //首次创建
		auto textWin = ShapeTextWin::get();
		textWin->setShape(this);
		isEditing = true;
		win->refresh();
	}
	else if (hoverDraggerIndex == 8) {
		isEditing = false;
		ShapeTextWin::get()->hide();
		win->refresh();
	}
}

void ShapeText::mouseUp(const float& x, const float& y)
{
	auto half{ draggerSize / 2 }, w{ rect.right - rect.left }, h{ rect.bottom - rect.top };
	draggers[0].left = rect.left - half;
	draggers[0].top = rect.top - half;
	draggers[0].right = rect.left + half;
	draggers[0].bottom = rect.top + half;


	draggers[1].left = rect.right - half;
	draggers[1].top = rect.top - half;
	draggers[1].right = rect.right + half;
	draggers[1].bottom = rect.top + half;


	draggers[2].left = rect.right - half;
	draggers[2].top = rect.bottom - half;
	draggers[2].right = rect.right + half;
	draggers[2].bottom = rect.bottom + half;

	draggers[3].left = rect.left - half;
	draggers[3].top = rect.bottom - half;
	draggers[3].right = rect.left + half;
	draggers[3].bottom = rect.bottom + half;
}

void ShapeText::mouseMove(const float& x, const float& y)
{
	auto half{ borderPadding / 2.f + win->dpi };//多个一个dpi，让范围更大点
	auto hoverIndex{-1};
	if (x >= rect.left - half && x <= rect.right + half && y >= rect.top - half && y <= rect.bottom + half)
	{
		if (x <= rect.left + half || x >= rect.right - half || y <= rect.top + half || y >= rect.bottom - half) {
			hoverIndex = 8;
		}
		else {
			hoverIndex = 1;
		}
	}
	else {
		hoverIndex = 0;
	}
	if (hoverIndex != hoverDraggerIndex)
	{
		hoverDraggerIndex = hoverIndex;
		win->refresh();
	}
}

void ShapeText::setCursor()
{
	if (hoverDraggerIndex == 8) {
		win->setCursor(IDC_SIZEALL);
	}
	else if (hoverDraggerIndex == 0) {
		win->setCursor(IDC_ARROW);
	}
	else if (hoverDraggerIndex == 1) {
		win->setCursor(IDC_IBEAM);
	}
}

void ShapeText::finishEdit()
{

}


