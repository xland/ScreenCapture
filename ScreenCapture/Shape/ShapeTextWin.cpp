#include "pch.h"
#include "../Win/WinPin.h"
#include "ShapeTextWin.h"
#include "ShapeText.h"
static constexpr int timerID{ 18 };
std::unique_ptr<ShapeTextWin> shapeTextWin;

ShapeTextWin::ShapeTextWin(const int& x, const int& y, const int& w, const int& h) : WinBase(x, y, w, h)
{
}

ShapeTextWin::~ShapeTextWin()
{

}

ShapeTextWin* ShapeTextWin::get()
{
	if (!shapeTextWin.get()) {
		shapeTextWin = std::make_unique<ShapeTextWin>(-99999, -99999, 120, 44);
		shapeTextWin->createWindow(WS_EX_TOPMOST);
        shapeTextWin->show();
		shapeTextWin->padding = 8.f * shapeTextWin->dpi;
	}
	return shapeTextWin.get();
}

void ShapeTextWin::setShape(ShapeText* shape)
{
    auto fontSize{ shape->fontSize * dpi };
	this->shape = shape;
    caretVisible = false;
    render->CreateSolidColorBrush(shape->color, shape->textBrush.GetAddressOf());
    render->CreateSolidColorBrush(D2D1::ColorF(0x99C9EF), textSelectionBgBrush.GetAddressOf());
    auto dwriteFactory = getWriteFactory();
    ComPtr<IDWriteTextFormat> textFormat;
    dwriteFactory->CreateTextFormat(L"Microsoft YaHei", nullptr,
        DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
        fontSize, L"", textFormat.GetAddressOf());
    dwriteFactory->CreateTextLayout(shape->text.data(), static_cast<UINT32>(shape->text.size()), textFormat.Get(), FLT_MAX, FLT_MAX, shape->textLayout.GetAddressOf());
    shape->textLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
    shape->textLayout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
    shape->textLayout->SetFontSize(shape->fontSize * dpi, { 0, static_cast<UINT32>(shape->text.length()) });
    DWRITE_TEXT_METRICS tm = {};
    shape->textLayout->GetMetrics(&tm);
    auto winW = (int)tm.widthIncludingTrailingWhitespace+padding*2;
    auto winH = (int)tm.height + padding*2;
    resize(winW, winH);
    move((int)(shape->win->x + shape->pressX-winW/2+padding), (int)(shape->win->y + shape->pressY-winH/2+padding));
	setCaretByMousePos(0, 0);
    refresh();
    setTimer(800, timerID);
}

void ShapeTextWin::onMouseDrag(const int& x, const int& y, const UINT_PTR& modifiers)
{

}

bool ShapeTextWin::onCursor()
{
	setCursor(IDC_IBEAM);
	return TRUE;
}

void ShapeTextWin::onPaint()
{
	//render->Clear(D2D1::ColorF(0xff6688, 0.8));
    auto r = D2D1::RectF(0, 0, w, h);
    render->DrawRectangle(r, shape->textBrush.Get(), 2.0f * dpi);
    D2D1_POINT_2F origin = { padding, padding };
    render->DrawTextLayout(origin, shape->textLayout.Get(), shape->textBrush.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
    if (caretVisible) {
        render->DrawLine(caretPos, { caretPos.x,caretPos.y + caretHeight }, shape->textBrush.Get(), 1.0f * dpi);
    }
}
void ShapeTextWin::onTimer(const UINT& timerId)
{
    if (timerId != timerID) return;
    if (isMouseDown) return;
    caretVisible = !caretVisible;
    refresh();
}
void ShapeTextWin::onMouseDown(const int& x, const int& y, bool isRight)
{
    SetFocus(hwnd);
    SetCapture(hwnd);
    BOOL isTrailingHit;
    BOOL isInside;
    DWRITE_HIT_TEST_METRICS caretMetrics;
    shape->textLayout->HitTestPoint(x - padding, y - padding, &isTrailingHit, &isInside, &caretMetrics);
    caretPos.x = caretMetrics.left + padding;
    auto textPos = caretMetrics.textPosition;
    if (isTrailingHit) {
        caretPos.x += caretMetrics.width;
        textPos += 1;
    }
    caretPos.y = caretMetrics.top + padding;
    caretHeight = caretMetrics.height;
    caretSelectionDown = textPos;
    caretSelectionStart = 0;
    caretSelectionEnd = 0;
    InvalidateRect(hwnd, nullptr, FALSE);
}

void ShapeTextWin::setCaretByMousePos(const float& x, const float& y)
{
    BOOL isTrailingHit;
    BOOL isInside;
    DWRITE_HIT_TEST_METRICS caretMetrics;
    shape->textLayout->HitTestPoint(x - padding, y - padding, &isTrailingHit, &isInside, &caretMetrics);
    caretPos.x = caretMetrics.left + padding;
    auto textPos = caretMetrics.textPosition;
    if (isTrailingHit) {
        caretPos.x += caretMetrics.width;
        textPos += 1;
    }
    caretPos.y = caretMetrics.top + padding;
    caretHeight = caretMetrics.height;
    caretSelectionDown = textPos;
    caretSelectionStart = 0;
    caretSelectionEnd = 0;
}
