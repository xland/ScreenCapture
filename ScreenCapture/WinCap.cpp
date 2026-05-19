#include "pch.h"
#include "WinCap.h"
#include "WinPix.h"
#include "Util.h"
#include "App.h"
#include "CutMask.h"
#include "WinToolMain.h"
#include "WinToolSub.h"

std::unique_ptr<WinCap> winCap;

WinCap::WinCap()
{

}

WinCap::~WinCap()
{

}

void WinCap::init()
{
	winCap = std::make_unique<WinCap>();
    winCap->initPosSize();
    winCap->createWindow();
    winCap->captureScreen();
    winCap->winPix = std::make_unique<WinPix>();
    winCap->cutMask = std::make_unique<CutMask>();
    ShowWindow(winCap->hwnd, SW_SHOW);
}
WinCap* WinCap::get()
{
    return winCap.get();
}
void WinCap::refresh()
{
    InvalidateRect(hwnd, nullptr, false);
}
ComPtr<ID2D1Bitmap1> WinCap::getImgData(const int& x, const int& y, const int& r, const int& b, bool hasShape)
{
    ComPtr<ID2D1Bitmap1> cpuImg;
    ComPtr<ID2D1DeviceContext> dc;
    render->QueryInterface(IID_PPV_ARGS(dc.GetAddressOf()));
    if (hasShape) {
        dc->SetTarget(screenImg.Get());
        dc->BeginDraw();
        paintShape(dc.Get());
        dc->EndDraw();
    }
    D2D1_BITMAP_PROPERTIES1 prop1{};
    prop1.pixelFormat = screenImg->GetPixelFormat();
    prop1.bitmapOptions = D2D1_BITMAP_OPTIONS_CPU_READ | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
    screenImg->GetDpi(&prop1.dpiX, &prop1.dpiY);
    dc->CreateBitmap(D2D1::SizeU(r - x, b - y), nullptr, 0, &prop1, cpuImg.GetAddressOf());
    auto rect = D2D1::RectU(x, y, r, b);
    auto start = D2D1::Point2U(0, 0);
    auto hr = cpuImg->CopyFromBitmap(&start, screenImg.Get(), &rect);
    return cpuImg;
}
void WinCap::changeDrawState(const std::wstring& state)
{
    if (state == L"close") {
        App::exit(2);
    }
    else if (state == L"save") {

    }
    else if (state == L"clipboard") {

    }
    else if (state == L"pin") {

    }
    else if (state == L"redo") {

    }
    else if (state == L"undo") {

    }
    if (drawState == state) {
        drawState = std::wstring{};
        toolSub->close();
    }
    else {
        drawState = state;
    }
}

void WinCap::initPosSize()
{
    x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    h = GetSystemMetrics(SM_CYVIRTUALSCREEN);
}
void WinCap::createWindow()
{
    WNDCLASSEX wx{};
    wx.lpfnWndProc = &WinCap::winMsg;
    wx.cbSize = sizeof(WNDCLASSEX);
    wx.style = CS_HREDRAW | CS_VREDRAW;
    wx.cbClsExtra = 0;
    wx.cbWndExtra = 0;
    wx.hInstance = GetModuleHandle(NULL);
    wx.hIcon = LoadIcon(wx.hInstance, (LPCTSTR)IDI_WINLOGO);
    wx.hCursor = LoadCursor(nullptr, IDC_CROSS);
    wx.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wx.lpszMenuName = nullptr;
    wx.lpszClassName = L"WinCap";
    wx.hIconSm = LoadIcon(wx.hInstance, (LPCTSTR)IDI_WINLOGO);
    auto r = RegisterClassEx(&wx);
    if (r == 0) {
        throw std::runtime_error("reg window class error");
    }
    hwnd = CreateWindowEx(NULL, wx.lpszClassName, NULL, WS_POPUP, x, y, w, h, NULL, NULL, wx.hInstance, NULL);
    SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    dpi = GetDpiForWindow(hwnd) / 96.0f;
    render = Util::createRender(hwnd, w, h);
    render->CreateSolidColorBrush(D2D1::ColorF(0xE81123), shapeBrush.GetAddressOf());
}
void WinCap::captureScreen()
{
    HDC hScreen = GetDC(NULL);
    HDC hDC = CreateCompatibleDC(hScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, w, h);
    auto oldObj = SelectObject(hDC, hBitmap);
    BOOL bRet = BitBlt(hDC, 0, 0, w, h, hScreen, x, y, SRCCOPY);
    ReleaseDC(NULL, hScreen);
    std::vector<BYTE> data(w * 4 * h);
    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = -h;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    GetDIBits(hDC, hBitmap, 0, h, data.data(), &bmi, DIB_RGB_COLORS);
    SelectObject(hDC, oldObj);
    DeleteDC(hDC);
    DeleteObject(hBitmap);
    D2D1_BITMAP_PROPERTIES1 props = {
        .pixelFormat{D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)},
        .dpiX{96.0f},
        .dpiY{96.0f},
        .bitmapOptions{D2D1_BITMAP_OPTIONS_TARGET},
    };
    ComPtr<ID2D1DeviceContext> dc;
    render->QueryInterface(IID_PPV_ARGS(dc.GetAddressOf()));
    dc->CreateBitmap(D2D1::SizeU(w, h), data.data(), w * 4, props, screenImg.GetAddressOf());
}
LRESULT WinCap::winMsg(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    auto self = reinterpret_cast<WinCap*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    if (!self) {
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    else if (msg == WM_ERASEBKGND)
    {
        return 1;
    }
    else if (msg == WM_PAINT)
    {
        self->onPaint();
    }
    else if (msg == WM_RBUTTONDOWN)
    {
        self->onMouseDown(LOWORD(lParam), HIWORD(lParam), true);
    }
    else if (msg == WM_LBUTTONDOWN) {
        self->onMouseDown(LOWORD(lParam), HIWORD(lParam), false);
    }
    else if (msg == WM_LBUTTONUP) {
        self->onMouseUp(LOWORD(lParam), HIWORD(lParam));
    }
    else if (msg == WM_MOUSEMOVE) {
        if (self->isMouseDown) {
            self->onMouseDrag(LOWORD(lParam), HIWORD(lParam));
        }
        else {
            self->onMouseMove(LOWORD(lParam), HIWORD(lParam));
        }
    }
    else if (msg == WM_MOUSELEAVE) {
        self->onMouseLeave();
    }
    //else if (msg == WM_KEYDOWN) {
    //    self->onKeyDown(wParam);
    //}
    else if (msg == WM_SETCURSOR) {
        if (self->onSetCursor()) return 1;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}
bool WinCap::onSetCursor()
{
    return (bool)toolMain.get();
}
void WinCap::onPaint()
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);
    render->BeginDraw();
    D2D1_RECT_F destRect = D2D1::RectF(0, 0, w, h);
    render->DrawBitmap(screenImg.Get(), destRect);
    paintShape(render.Get());
    cutMask->paint();
    render->EndDraw();
    EndPaint(hwnd, &ps);
}

void WinCap::paintShape(ID2D1RenderTarget* render)
{
    //for (const auto& shape : shapes)
    //{
    //    if (shape.state == 2) break;
    //    if (shape.type == 0) {
    //        auto rect = D2D1::RectF(std::min(shape.x1, shape.x2), std::min(shape.y1, shape.y2),
    //            std::max(shape.x1, shape.x2), std::max(shape.y1, shape.y2));
    //        render->DrawRectangle(rect, shapeBrush.get(), 2 * dpi);
    //    }
    //    else if (shape.type == 1) {
    //        auto g = Util::createArrow({ shape.x1, shape.y1 }, { shape.x2, shape.y2 });
    //        render->FillGeometry(g.get(), shapeBrush.get());
    //    }
    //}
}

void WinCap::onMouseMove(const int& x, const int& y) {
    if (!mouseIn)
    {
        mouseIn = true;
        Util::trackMouse(hwnd);
    }
    if (drawState.empty()) {
        if (toolMain.get()) {
            cutMask->changeCursor(x, y);
        }
        else {
            cutMask->highlight(x, y);
            winPix->move(x, y);
        }
    }
    else {
        SetCursor(LoadCursor(NULL, IDC_CROSS));
    }

}
void WinCap::onMouseDrag(const int& x, const int& y)
{
    if (drawState.empty()) {
        if (toolMain.get()) {
            cutMask->changeRect(x, y);
        }
        else {
            cutMask->makeRect(x, y);
        }
    }
    else {
        //Shape& last = shapes.back();
        //last.x2 = x;
        //last.y2 = y;
        //last.state = 1;
        refresh();
    }
}
void WinCap::onMouseDown(const int& x, const int& y, bool isRight)
{
    isMouseDown = true;
    if (isRight) {
        App::exit(2);
        return;
    }
    if (drawState.empty()) {
        if (winPix.get()) {
            winPix->close();
            winPix.reset();
        }
        if (toolMain.get()) {
            toolMain->hide();
            cutMask->startChangeRect(x, y);
        }
        else {
            cutMask->startMakeRect(x, y);
        }
    }
    else {
        //std::erase_if(shapes, [](const Shape& s) { return s.state == 2; });
        //Shape shape;
        //shape.x1 = x;
        //shape.y1 = y;
        //shape.x2 = x;
        //shape.y2 = y;
        //if (drawState == L"rect") {
        //    shape.type = 0;
        //}
        //else if (drawState == L"arrow") {
        //    shape.type = 1;
        //}
        //shape.state = 0;
        //shapes.push_back(std::move(shape));
    }
}
void WinCap::onMouseUp(const int& x, const int& y)
{
    isMouseDown = false;
    if (!toolMain.get()) {
        toolMain = std::make_unique<WinToolMain>();
    }
    if (drawState.empty()) {
        toolMain->show();
    }
    else {
        //Shape& last = shapes.back();
        //if (last.state == 0) {
        //    shapes.pop_back();
        //}
    }
}
void WinCap::onMouseLeave()
{
    cutMask->cursorIndex = -1;
    Util::trackMouse(hwnd, true);
    mouseIn = false;
}