#include "pch.h"
#include "App.h"
#include "Win/WinPin.h"
#include "Tool/ToolMain.h"
#include "Tool/ToolSub.h"
#include "Tool/ToolSubSlider.h"
#include "Tool/ToolSubColor.h"
#include "Util.h"
#include "ShapeText.h"
#include "ShapeTextWin.h"

ShapeText::ShapeText(WinPin* win) :ShapeBase(win), borderPadding{ 6.f * win->dpi }
{
	setAttr();
	float dashes[] = { 2.0f, 2.0f };
	App::getD2D()->CreateStrokeStyle(
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
		winX += spanX;
		winY += spanY;
		pressX = x;
		pressY = y;
	}
}

void ShapeText::mouseDown(const float& x, const float& y)
{
	if (hoverDraggerIndex == -1) { //首次创建
		auto textWin = ShapeTextWin::get();
		winX = win->x + x;
		winY = win->y + y;
		textWin->setShape(this);
		isEditing = true;
		win->refresh();
	}
	else if (hoverDraggerIndex == 8) { //拖拽
		pressX = x;
		pressY = y;
		isEditing = false;
		ShapeTextWin::get()->hide();
		win->refresh();
	}
	else if (hoverDraggerIndex == 1) { //编辑
		pressX = x;
		pressY = y;
		auto textWin = ShapeTextWin::get();
		textWin->setShape(this);
		isEditing = true;
		win->refresh();
	}
	else if (hoverDraggerIndex == 0) { //结束编辑
		auto textWin = ShapeTextWin::get();
		textWin->hide();//会触发onBlur事件
	}
}

void ShapeText::mouseUp(const float& x, const float& y)
{
}

void ShapeText::mouseMove(const float& x, const float& y)
{
	hoverDraggerIndex = -1;
	auto half{ borderPadding / 2.f + win->dpi };//多个一个dpi，让范围更大点
	if (x >= rect.left - half && x <= rect.right + half && y >= rect.top - half && y <= rect.bottom + half)
	{
		if (x <= rect.left + half || x >= rect.right - half || y <= rect.top + half || y >= rect.bottom - half) {
			hoverDraggerIndex = 8;
		}
		else {
			hoverDraggerIndex = 1;
		}
	}
	else if(isEditing) {
		hoverDraggerIndex = 0; //必须让当前形状是shapeHover
	}
}

void ShapeText::setCursor()
{
	if (hoverDraggerIndex == 8) {
		win->setCursor(IDC_SIZEALL);
	}
	else if (hoverDraggerIndex == 1 ) {
		win->setCursor(IDC_IBEAM);
	}
	else if (hoverDraggerIndex == 0) {
		win->setCursor(IDC_ARROW);
	}
}

void ShapeText::finishEdit()
{

}

void ShapeText::setTextLayout()
{
	auto textWin = ShapeTextWin::get();
	textLayout.Reset();
	auto dwriteFactory = App::getWriter();
	dwriteFactory->CreateTextLayout(text.data(), static_cast<UINT32>(text.size()), textWin->textFormat.Get(), FLT_MAX, FLT_MAX, textLayout.GetAddressOf());
	textLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	textLayout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
	textLayout->SetFontSize(fontSize, { 0, static_cast<UINT32>(text.length()) });
	textLayout->SetFontWeight(isBold ? DWRITE_FONT_WEIGHT_BOLD : DWRITE_FONT_WEIGHT_NORMAL, { 0, static_cast<UINT32>(text.length()) });
	textLayout->SetFontStyle(isItalic ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL, { 0, static_cast<UINT32>(text.length()) });

	DWRITE_TEXT_METRICS tm = {}; //文本布局后度量信息的结构体
	textLayout->GetMetrics(&tm);
	auto winW = (int)tm.widthIncludingTrailingWhitespace; //包含尾随空白字符（trailing whitespace）在内的文本行宽度
	auto winH = (int)tm.height;
	textWin->resize(winW, winH);

	POINT pos{ winX, winY };
	ScreenToClient(win->hwnd, &pos);
	rect = D2D1::RectF(pos.x - borderPadding, pos.y - borderPadding,
		pos.x + winW + borderPadding,
		pos.y + winH + borderPadding);

	win->refresh();
}

void ShapeText::setAttr()
{
	auto toolSub = win->toolSub.get();
	color = toolSub->colorer->getSelectedColor();
	fontSize = toolSub->slider->val;
	isBold = toolSub->selectIndex == 0;
	isItalic = toolSub->selectIndex2 == 1;
}


