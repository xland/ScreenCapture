#include "pch.h"
#include "App.h"
#include "Util.h"
#include "WinLong.h"
#include "Tool/ToolLong.h"
#include "WinCutMask.h"

std::unique_ptr<WinLong> winLong;
static constexpr UINT scrollMsgId = 18;
static constexpr UINT scrollEndMsgId = 19;

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
    winLong->createWindow();//WS_EX_TOPMOST
    winLong->initRes();
    POINT pt;
    GetCursorPos(&pt);
    winLong->cutMask = std::make_unique<WinCutMask>(winLong.get());
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
    if (isShowStartBtn) {
        render->FillEllipse(D2D1::Ellipse(D2D1::Point2F(circleCenter.x, circleCenter.y), startCircleR, startCircleR), bgBrush.Get());
        render->DrawTextLayout({ circleCenter.x- startCircleR, circleCenter.y - startCircleR }, layoutText.Get(), textBrush.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
    }
    if (imgPreview.Get()) {
        float imgW, imgH;
        auto imgSize = imgPreview->GetSize();
        if (imgSize.width < tool->w) {
            imgW = tool->w;
            auto scaleNum{ tool->w / imgSize.width };
            imgH = imgSize.height * scaleNum;
        }
        else {
            imgW = tool->w;
            auto scaleNum{ imgSize.width/tool->w };
            imgH = imgSize.height / scaleNum;
        }
        POINT pos{ tool->x,tool->y - imgH };
        ScreenToClient(hwnd, &pos);
        D2D1_RECT_F destRect = D2D1::RectF(pos.x, pos.y, imgW, imgH);
        render->DrawBitmap(imgPreview.Get(), destRect);
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
    if (!isFinishCutMask) {
        isFinishCutMask = true;
        return;
    }
    if (isShowStartBtn) {
        isScrolling = true;
        hollowWin();
        makeTool();
        capStep();
    }
}

void WinLong::onTimer(const UINT& timerId)
{
    if (timerId == scrollMsgId) {
        POINT pt;
        GetCursorPos(&pt);
        auto tarHwnd = WindowFromPoint(pt);
        if (targetHwnd == nullptr) {
            targetHwnd = tarHwnd;
        }
        if (tarHwnd != targetHwnd) return;
        killTimer(scrollMsgId);
        INPUT input = { 0 };
        input.type = INPUT_MOUSE;
        input.mi.dwFlags = MOUSEEVENTF_WHEEL;
        input.mi.mouseData = -WHEEL_DELTA;
        SendInput(1, &input, sizeof(INPUT));
        setTimer(88, scrollEndMsgId);
    }
	else if (scrollEndMsgId == timerId) {
		killTimer(scrollEndMsgId);
        capStep();
	}
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
        GetCursorPos(&circleCenter);
		ScreenToClient(hwnd, &circleCenter);
        auto& r = cutMask->maskRect;
        auto rect = D2D1::RectU((UINT32)r.left, (UINT32)r.top, (UINT32)r.right, (UINT32)r.bottom);
        if (circleCenter.x > r.left && circleCenter.y > r.top && circleCenter.x < r.right && circleCenter.y < r.bottom) {
            SetCursor(NULL);
            isShowStartBtn = true;
        }
        else {
            setCursor(IDC_ARROW);
            isShowStartBtn = false;
        }
    }
    else {
        setCursor(IDC_CROSS);
        isShowStartBtn = false;
    }
    return TRUE;
}

void WinLong::capStep()
{
    auto& maskRect = cutMask->maskRect;
    auto maskW{ maskRect.right - maskRect.left }, maskH{ maskRect.bottom - maskRect.top };
    POINT p{ .x{(int)maskRect.left},.y{(int)maskRect.top} };
    ClientToScreen(hwnd, &p);
    if (imgData.empty()) { 
        //首次执行截取
        imgData = Util::captureScreen(p.x, p.y, maskW, maskH);
    }
    else {
        auto data = Util::captureScreen(p.x, p.y, maskW, maskH);//截图区域的像素数据
        //与imgData融合，这里的代码还没写
        //.....
    }
    imgPreview.Reset();
    D2D1_BITMAP_PROPERTIES1 props = {
        .pixelFormat{D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)},
        .dpiX{96.0f}, .dpiY{96.0f}, .bitmapOptions{D2D1_BITMAP_OPTIONS_TARGET}
    };
    render->CreateBitmap(D2D1::SizeU(w, h), imgData.data(), w * 4, props, imgPreview.GetAddressOf());
    refresh();
    //static int name{ 1 };
    //auto path = std::format(L"D:\\{}.png", name);
    //Util::saveToFile(path, (int)maskW, (int)maskH, imgData.data());
    //name += 1;
    setTimer(500, scrollMsgId);
}

void WinLong::hollowWin()
{
    HRGN rgn1 = CreateRectRgn(0, 0, w, h);
    auto& r = cutMask->maskRect;
    HRGN rgn2 = CreateRectRgn(r.left, r.top, r.right, r.bottom);
    CombineRgn(rgn1, rgn1, rgn2, RGN_DIFF);
    SetWindowRgn(hwnd, rgn1, true);
}

void WinLong::makeTool()
{
    auto btnSize{ 32.f * dpi };
    auto toolW{ btnSize * 4 };
    POINT pos{ 0,0 };
    if (w - cutMask->maskRect.right - dpi < toolW) {
        pos.x = cutMask->maskRect.left - toolW - dpi;
    }
    else {
        pos.x = cutMask->maskRect.right + dpi;
    }
    pos.y = cutMask->maskRect.bottom - btnSize;
    ClientToScreen(hwnd, &pos);
    tool = std::make_unique<ToolLong>(pos.x, pos.y, toolW, btnSize, this);
    tool->createWindow(WS_EX_TOPMOST | WS_EX_NOACTIVATE);
    tool->show();
}
