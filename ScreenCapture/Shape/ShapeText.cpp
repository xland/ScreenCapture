#include "pch.h"
#include "../Win/WinPin.h"
#include "../Win/WinToolMain.h"
#include "../Win/WinToolSub.h"
#include "../Util.h"
#include "ShapeText.h"
#include "ShapeTextWin.h"

ShapeText::ShapeText(WinPin* win) :ShapeBase(win)
{
	auto toolSub = WinToolSub::get();
	color = toolSub->getSelectedColor();
	fontSize = toolSub->sliderVal;
	isBold = toolSub->selectIndex == 0;
	isItalic = toolSub->selectIndex == 1;
	auto dwriteFactory = win->getWriteFactory();
	dwriteFactory->CreateTextFormat(L"Microsoft YaHei", nullptr,
		DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
		12.f, L"", textFormat.GetAddressOf());
}

ShapeText::~ShapeText()
{

}

void ShapeText::paint()
{
}

void ShapeText::paintDragger()
{
}

void ShapeText::mouseDrag(const float& x, const float& y, const UINT_PTR& modifiers)
{
}

void ShapeText::mouseDown(const float& x, const float& y)
{
	if (hoverDraggerIndex == -1) { //首次创建
		pressX = x;
		pressY = y;
		auto textWin = ShapeTextWin::get();
		textWin->setShape(this);
		textWin->show();
	}
}

void ShapeText::mouseUp(const float& x, const float& y)
{

}

void ShapeText::mouseMove(const float& x, const float& y)
{
    if (Util::isInRect(rect, x, y))
    {
		hoverDraggerIndex = 8;
    }
}

void ShapeText::setCursor()
{
	if (hoverDraggerIndex == 8) {
		win->setCursor(IDC_IBEAM);
	}
}

void ShapeText::setTextLayout()
{
	textLayout.Reset();
	auto dwriteFactory = win->getWriteFactory();
	dwriteFactory->CreateTextLayout(text.data(), static_cast<UINT32>(text.size()), textFormat.Get(), FLT_MAX, FLT_MAX, textLayout.GetAddressOf());
	textLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	textLayout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
	textLayout->SetFontSize(fontSize, { 0, static_cast<UINT32>(text.length()) });
}
