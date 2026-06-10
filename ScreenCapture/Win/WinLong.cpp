#include "pch.h"
#include "App.h"
#include "Util.h"
#include "WinLong.h"
#include "CutMask.h"

std::unique_ptr<WinLong> winLong;

WinLong::WinLong(const int& x, const int& y, const int& w, const int& h) : WinBase(x, y, w, h)
{
}

WinLong::~WinLong()
{

}

void WinLong::init()
{
    WinLong::release();
    auto x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    auto y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    auto w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    auto h = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    winLong = std::make_unique<WinLong>(x, y, w, h);
    winLong->createWindow(WS_EX_TOPMOST);
    winLong->initRes();
    POINT pt;
    GetCursorPos(&pt);
    winLong->cutMask = std::make_unique<CutMask>(winLong.get());
    winLong->show();
    UpdateWindow(winLong->hwnd);

}
WinLong* WinLong::get()
{
    return winLong.get();
}

void WinLong::release()
{
    if (winLong.get()) {
        winLong->close();
        winLong.reset();
    }
}


void WinLong::onPaint()
{
    render->Clear(0);
    cutMask->paint();
    if (isFinishCutMask) {
        POINT pt;
        GetCursorPos(&pt);
        render->FillEllipse(D2D1::Ellipse(D2D1::Point2F(pt.x, pt.y), startCircleR, startCircleR), bgBrush.Get());
        render->DrawTextLayout({ pt.x- startCircleR, pt.y - startCircleR }, layoutText.Get(), textBrush.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
    }
}

void WinLong::onMouseMove(const int& x, const int& y) {
    if (isFinishCutMask) {
        refresh();
    }
    else {
        cutMask->highlight(x, y);
    }
}
void WinLong::onMouseDrag(const int& x, const int& y, const UINT_PTR& modifiers)
{
    if (isFinishCutMask)  return;
    cutMask->makeRect(x, y);
}
void WinLong::onMouseDown(const int& x, const int& y, bool isRight)
{
    if (isRight) {
        App::exit(2);
        return;
    }
    if (isFinishCutMask)  return;
    cutMask->startMakeRect(x, y);
}
void WinLong::onMouseUp(const int& x, const int& y)
{
    isFinishCutMask = true;;
    //HRGN rgn1 = CreateRectRgn(0, 0, w, h);
 //   auto& r = cutMask->maskRect;
 //   HRGN rgn2 = CreateRectRgn(r.left, r.top, r.right, r.bottom);
 //   CombineRgn(rgn1, rgn1, rgn2, RGN_DIFF);
 //   SetWindowRgn(hwnd, rgn1, true);
	//winStart = std::make_unique<WinStart>(x, y, 60*dpi, 60 * dpi);
}

void WinLong::initRes()
{
    startCircleR *= dpi;
    render->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), textBrush.GetAddressOf());
    render->CreateSolidColorBrush(D2D1::ColorF(0x000000, 0.68f), bgBrush.GetAddressOf());
    ComPtr<IDWriteTextFormat> textFormat;
    auto writer = App::getWriter();
    writer->CreateTextFormat(L"Microsoft YaHei", nullptr,
        DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
        16 * dpi, L"", textFormat.GetAddressOf());
    std::wstring text = L"开始";
    writer->CreateTextLayout(text.data(), (UINT32)text.length(), textFormat.Get(), startCircleR*2, startCircleR*2, layoutText.GetAddressOf());
    layoutText->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    layoutText->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
}

BOOL WinLong::onCursor()
{
    if (isFinishCutMask) {
        SetCursor(NULL);
    }
    else {
        setCursor(IDC_CROSS);
    }
    return TRUE;
}
