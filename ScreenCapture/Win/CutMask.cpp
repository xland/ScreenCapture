#include "pch.h"
#include "App.h"
#include "Util.h"
#include "WinCap.h"
#include "CutMask.h"

CutMask::CutMask() 
{
    auto win = WinCap::get();
    auto render = win->render.Get();
    render->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), brushText.GetAddressOf());
    render->CreateSolidColorBrush(D2D1::ColorF(0x000000, 0.46f), brushBg.GetAddressOf());
    render->CreateSolidColorBrush(D2D1::ColorF(0x1677ff), brushBorder.GetAddressOf());
    auto writer = App::getWriter();
    writer->CreateTextFormat(L"Microsoft YaHei", nullptr,
        DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
        15, L"", textFormat.GetAddressOf());
    initWinRect();
}

CutMask::~CutMask() {

}

bool CutMask::highlight(const int& x, const int& y)
{
    for (auto& rect : winRect)
    {
        if (x>rect.left&&y>rect.top&&x<rect.right&&y<rect.bottom) {
            if (maskRect.left != rect.left||maskRect.top != rect.top || 
                maskRect.right != rect.right || maskRect.bottom != rect.bottom) {
                maskRect = rect;
                WinCap::get()->refresh();
                return true;
            }
            break;
        }
    }
    return false;
}

void CutMask::initWinRect()
{
    winRect.clear();
    EnumWindows([](HWND hwnd, LPARAM lparam)
        {
            if (!hwnd) return TRUE;
            if (!IsWindowVisible(hwnd)) return TRUE;
            if (IsIconic(hwnd)) return TRUE;
            if (GetWindowTextLength(hwnd) < 1) return TRUE;
            RECT rect;
            DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &rect, sizeof(RECT));
            if (rect.right - rect.left <= 6 || rect.bottom - rect.top <= 6) return TRUE;
            auto winCap = WinCap::get();
            if (rect.left < winCap->x) rect.left = winCap->x;
            if (rect.top < winCap->y) rect.top = winCap->y;
            if (rect.right > winCap->x + winCap->w) rect.right = winCap->x + winCap->w;
            if (rect.bottom > winCap->y + winCap->h) rect.bottom = winCap->y + winCap->h;
            auto x = (rect.left - winCap->x);
            auto y = (rect.top - winCap->y);
            auto r = (rect.right - winCap->x);
            auto b = (rect.bottom - winCap->y);
            auto self = (CutMask*)lparam;
            self->winRect.push_back(D2D1::RectF(x, y, r, b));
            return TRUE;
        }, (LPARAM)this);
}
void CutMask::startMakeRect(const int& x, const int& y)
{
    pressX = x;
    pressY = y;
}

void CutMask::makeRect(const int& x, const int& y)
{
    auto [left, right] = std::minmax(pressX, x);
    auto [top, bottom] = std::minmax(pressY, y);
    maskRect.left = left;
    maskRect.right = right;
    maskRect.top = top;
    maskRect.bottom = bottom;
    WinCap::get()->refresh();
}

void CutMask::paint()
{
    auto win = WinCap::get();
    auto render = win->render.Get();
    D2D1_RECT_F destRect = D2D1::RectF(0, 0, win->w, win->h);

    auto d2d = App::getD2D();
    ComPtr<ID2D1RectangleGeometry> outerGeometry;
    d2d->CreateRectangleGeometry(destRect, outerGeometry.GetAddressOf());
    ComPtr<ID2D1RectangleGeometry> innerGeometry;
    d2d->CreateRectangleGeometry(maskRect, innerGeometry.GetAddressOf());
    ID2D1Geometry* geometries[] = { outerGeometry.Get(), innerGeometry.Get() };
    ComPtr<ID2D1GeometryGroup> geo;
    d2d->CreateGeometryGroup(D2D1_FILL_MODE_ALTERNATE,geometries, ARRAYSIZE(geometries), geo.GetAddressOf());


    render->FillGeometry(geo.Get(), brushBg.Get());
    auto r = D2D1::RectF(maskRect.left - 2, maskRect.top - 2, maskRect.right + 2, maskRect.bottom + 2);
    render->DrawRectangle(r, brushBorder.Get(), 4.f);

    auto str = std::format(L"X:{} Y:{} R:{} B:{} W:{} H:{}",
        maskRect.left,maskRect.top,maskRect.right,maskRect.bottom,
        maskRect.right - maskRect.left,maskRect.bottom-maskRect.top);    
    auto writer = App::getWriter();
    winrt::com_ptr<IDWriteTextLayout> layout;
    writer->CreateTextLayout(str.data(),static_cast<UINT32>(str.size()), textFormat.Get(), FLT_MAX, FLT_MAX, layout.put());
    layout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    layout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    layout->SetFontSize(10*win->dpi, { 0, static_cast<UINT32>(str.length()) });
    DWRITE_TEXT_METRICS tm = {};
    layout->GetMetrics(&tm);

    
    float paddingTop{ 3 * win->dpi }, paddingLeft{ 5 * win->dpi }, margin{5*win->dpi};
    D2D1_RECT_F rect = D2D1::RectF(maskRect.left, maskRect.top - margin - tm.height - paddingTop*2, maskRect.left+tm.width+ paddingLeft *2, maskRect.top - margin);

    if (rect.top < win->y) {
        auto h = rect.bottom - rect.top;
        auto w = rect.right - rect.left;
        rect.top = maskRect.top + margin/2;
        rect.bottom = rect.top + h;
        rect.left = maskRect.left + margin;
        rect.right = rect.left + w;
    }


    layout->SetMaxWidth(rect.right-rect.left );
    layout->SetMaxHeight(rect.bottom - rect.top);
    D2D1_ROUNDED_RECT rr = D2D1::RoundedRect(rect, paddingTop, paddingTop);
    render->FillRoundedRectangle(rr, brushBg.Get());
    D2D1_POINT_2F origin = { rect.left, rect.top };
    render->DrawTextLayout(origin, layout.get(), brushText.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);


    //通过这种方式可以擦掉指定的区域
    //render->PushAxisAlignedClip(cutMask->maskRect, D2D1_ANTIALIAS_MODE_ALIASED);
    //render->Clear(D2D1::ColorF(0, 0, 0, 0));   // Alpha=0 即“擦掉”
    //render->PopAxisAlignedClip();
}
