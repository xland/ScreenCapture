#include "pch.h"
#include "Util.h"
#include "WinCap.h"
#include "CutMask.h"

CutMask::CutMask() 
{
    auto render = WinCap::get()->render.Get();
    //textLayout = Util::getTextLayout(L"\ue8e8", fontSize, btnW, h);
    render->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), brushText.GetAddressOf());
    render->CreateSolidColorBrush(D2D1::ColorF(0x000000, 0.46f), brushBg.GetAddressOf());
    render->CreateSolidColorBrush(D2D1::ColorF(0x1677ff), brushBorder.GetAddressOf());
    auto dwriteFactory = Util::getWriteFactory();
    dwriteFactory->CreateTextFormat(L"Microsoft YaHei", nullptr,
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

void CutMask::changeCursor(const int& x, const int& y)
{
    int index{ -1 };
    if (x <= maskRect.left && y <= maskRect.top) {   
        index = 0;
    }
    else if (x >= maskRect.right && y <= maskRect.top) { 
        index = 2;
    }
    else if (x <= maskRect.left && y >= maskRect.bottom) { 
        index = 6;
    }
    else if (x >= maskRect.right && y >= maskRect.bottom) {
        index = 8;
    }
    else if (y < maskRect.top) { 
        index = 1;
    }
    else if (y > maskRect.bottom) {
        index = 7;
    }
    else if (x < maskRect.left) { 
        index = 3;
    }
    else if (x > maskRect.right) {
        index = 5;
    }
    else { 
        index = 4;
    }
    if (index == cursorIndex) return;
    switch (index)
    {
    case 0:
    case 8: {
        SetCursor(LoadCursor(NULL, IDC_SIZENWSE)); 
        break; // 左上,右下
    }
    case 1:
    case 7: {
        SetCursor(LoadCursor(NULL, IDC_SIZENS));   
        break; // 上 下
    }
    case 2:
    case 6: {
        SetCursor(LoadCursor(NULL, IDC_SIZENESW)); 
        break; // 右上 左下
    }
    case 3:
    case 5: {
        SetCursor(LoadCursor(NULL, IDC_SIZEWE));   
        break; // 左 右
    }
    case 4: {
        SetCursor(LoadCursor(NULL, IDC_SIZEALL));  
        break; // 中心
    }
    }
    cursorIndex = index;
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
    originalRect = maskRect;
    changeRect(x, y);
}
void CutMask::startChangeRect(const int& x, const int& y)
{
    switch (cursorIndex)
    {
    case 0: // 左上角
        maskRect.left = x;
        maskRect.top = y;
        break;
    case 1: // 上边
        maskRect.top = y;
        break;
    case 2: // 右上角
        maskRect.right = x;
        maskRect.top = y;
        break;
    case 3: // 左边
        maskRect.left = x;
        break;
    case 5: // 右边
        maskRect.right = x;
        break;
    case 6: // 左下角
        maskRect.left = x;
        maskRect.bottom = y;
        break;
    case 7: // 下边
        maskRect.bottom = y;
        break;
    case 8: // 右下角
        maskRect.right = x;
        maskRect.bottom = y;
        break;
    }
    pressX = x;
    pressY = y;
    originalRect = maskRect;
    WinCap::get()->refresh();
}
void CutMask::changeRect(const int& x, const int& y)
{
    int dx = x - pressX;
    int dy = y - pressY;
    D2D1_RECT_F newRect = originalRect;
    if (cursorIndex == 4) {
        newRect.left += dx;
        newRect.top += dy;
        newRect.right += dx;
        newRect.bottom += dy;
    }
    else {
        switch (cursorIndex)
        {
        case 0: // 左上角
            newRect.left += dx;
            newRect.top += dy;
            break;
        case 1: // 上边
            newRect.top += dy;
            break;
        case 2: // 右上角
            newRect.right += dx;
            newRect.top += dy;
            break;
        case 3: // 左边
            newRect.left += dx;
            break;
        case 5: // 右边
            newRect.right += dx;
            break;
        case 6: // 左下角
            newRect.left += dx;
            newRect.bottom += dy;
            break;
        case 7: // 下边
            newRect.bottom += dy;
            break;
        case 8: // 右下角
            newRect.right += dx;
            newRect.bottom += dy;
            break;
        }
    }
    auto win = WinCap::get();
    bool clamped = false;
    if (newRect.left < 0) {
        newRect.right -= newRect.left;
        newRect.left = 0;
        clamped = true;
    }
    if (newRect.top < 0) {
        newRect.bottom -= newRect.top;
        newRect.top = 0;
        clamped = true;
    }
    if (newRect.right > win->w) {
        newRect.left -= newRect.right-win->w;
        newRect.right = win->w;
        clamped = true;
    }
    if (newRect.bottom > win->h) {
        newRect.top -= newRect.bottom-win->h;
        newRect.bottom = win->h;
        clamped = true;
    }
    if (clamped && cursorIndex == 4) { 
        float actualDx = newRect.left - originalRect.left;
        float actualDy = newRect.top - originalRect.top; 
        pressX = static_cast<int>(x - actualDx);
        pressY = static_cast<int>(y - actualDy);
    }
    if (maskRect.left == newRect.left && maskRect.top == newRect.top &&
        maskRect.right == newRect.right && maskRect.bottom == newRect.bottom) {
        return;
    }
    maskRect = newRect;
    WinCap::get()->refresh();
}

void CutMask::makeRect(const int& x, const int& y)
{
    maskRect.left = std::min(pressX,x);
    maskRect.top = std::min(pressY,y);
    maskRect.right = std::max(pressX,x);
    maskRect.bottom = std::max(pressY,y);
    WinCap::get()->refresh();
}

void CutMask::paint()
{
    auto win = WinCap::get();
    auto render = win->render.Get();
    D2D1_RECT_F destRect = D2D1::RectF(0, 0, win->w, win->h);
    auto d2d = Util::getD2D();
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
    auto dwriteFactory = Util::getWriteFactory();
    winrt::com_ptr<IDWriteTextLayout> layout;
    dwriteFactory->CreateTextLayout(str.data(),static_cast<UINT32>(str.size()), textFormat.Get(), FLT_MAX, FLT_MAX, layout.put());
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
