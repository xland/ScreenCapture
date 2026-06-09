#include "pch.h"
#include "App.h"
#include "Util.h"
#include "WinStart.h"

WinStart::WinStart(const int& x, const int& y, const int& w, const int& h) : WinBase(x, y, w, h)
{
	createWindow(WS_EX_TOPMOST);
	render->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), textBrush.GetAddressOf());
	render->CreateSolidColorBrush(D2D1::ColorF(0x000000,0.6f), bgBrush.GetAddressOf());
	ComPtr<IDWriteTextFormat> textFormat;
	auto writer = App::getWriter();
	writer->CreateTextFormat(L"Microsoft YaHei", nullptr,
		DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
		12*dpi, L"", textFormat.GetAddressOf());
	std::wstring text = L"开始";
	writer->CreateTextLayout(text.data(), (UINT32)text.length(), textFormat.Get(), w, w, layoutText.GetAddressOf());
	layoutText->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	layoutText->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	show();
	POINT pt;
	GetCursorPos(&pt);
	move(pt.x-w/2, pt.y-h/2);
	setTimer(600, WM_APP+100);
}

WinStart::~WinStart()
{

}

void WinStart::onPaint()
{
	render->Clear(0);
	auto r{ w / 2.f };
	render->FillEllipse(D2D1::Ellipse(D2D1::Point2F(r, r), r,r), bgBrush.Get());
	render->DrawTextLayout({ 0,0 }, layoutText.Get(), textBrush.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
}

void WinStart::onMouseMove(const int& x, const int& y) {
	SetCursor(NULL);
	POINT pt;
	GetCursorPos(&pt);
	move(pt.x - w / 2, pt.y - h / 2);
}
void WinStart::onMouseDrag(const int& x, const int& y, const UINT_PTR& modifiers)
{

}
void WinStart::onMouseDown(const int& x, const int& y, bool isRight)
{
	if (isRight) {
		ReleaseCapture();
		close();
	}
}
void WinStart::onMouseUp(const int& x, const int& y)
{

}

BOOL WinStart::onCursor()
{
	SetCursor(NULL);
	return TRUE;
}
