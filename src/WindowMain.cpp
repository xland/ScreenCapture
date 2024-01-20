#include "WindowMain.h"
#include <Windows.h>
#include <windowsx.h>
#include <dwmapi.h>
#include "include/core/SkBitmap.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkStream.h"
#include "include/encode/SkPngEncoder.h"

#include "App.h"
#include "CutMask.h"
#include "ToolMain.h"
#include "ToolSub.h"
#include "Recorder.h"
#include "PixelInfo.h"
#include "ColorBlender.h"

WindowMain::WindowMain()
{
    initSize();
    CutMask::Init(w,h);
    PixelInfo::Init();
    shotScreen();
    initWindow();
    state = State::start;
}

WindowMain::~WindowMain()
{
    delete PixelInfo::Get();
    delete CutMask::Get();
    ColorBlender::Reset();
}

void WindowMain::Save(const std::string& filePath)
{
    Recorder::Get()->FinishPaint();
    auto rect = CutMask::GetCutRect();
    auto img = surfaceBase->makeImageSnapshot(SkIRect::MakeLTRB(rect.fLeft, rect.fTop, rect.fRight, rect.fBottom));    
    SkPixmap pixmap;
    img->peekPixels(&pixmap);
    SkPngEncoder::Options option;
    SkFILEWStream stream(filePath.data());
    SkPngEncoder::Encode(&stream, pixmap, option);
    stream.flush();
    App::Quit(6);
}

void WindowMain::SaveToClipboard()
{
    Recorder::Get()->FinishPaint();
    auto rect = CutMask::GetCutRect();
    HDC ScreenDC = GetDC(NULL);
    HDC hMemDC = CreateCompatibleDC(ScreenDC);
    HBITMAP hBitmap = CreateCompatibleBitmap(ScreenDC, rect.width(), rect.height());
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
    float x{ this->x + rect.fLeft }, y{ this->y + rect.fTop };
    auto w{ rect.width() }, h{ rect.height() };
    StretchBlt(hMemDC, 0, 0, w, h, ScreenDC, x, y, w, h, SRCCOPY);
    hBitmap = (HBITMAP)SelectObject(hMemDC, hOldBitmap);
    DeleteDC(hMemDC);
    DeleteObject(hOldBitmap);
    if (!OpenClipboard(hwnd)) {
        MessageBox(NULL, L"Failed to open clipboard when save to clipboard.", L"Error", MB_OK | MB_ICONERROR);
        return;
    }
    EmptyClipboard();
    SetClipboardData(CF_BITMAP, hBitmap);
    CloseClipboard();
    ReleaseDC(NULL, ScreenDC);
    App::Quit(7);
}

void WindowMain::initCanvas()
{
    SkImageInfo info = SkImageInfo::MakeN32Premul(w, h);
    surfaceBase = SkSurfaces::Raster(info);
    surfaceBack = SkSurfaces::Raster(info);
    surfaceFront = SkSurfaces::Raster(info);
    pixBase = new SkPixmap();
    surfaceBase->peekPixels(pixBase);
    pixBack = new SkPixmap();
    surfaceBack->peekPixels(pixBack);
}

LRESULT WindowMain::wndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg)
    {
    case WM_LBUTTONDOWN:
    {
        IsMouseDown = true;
        auto x = GET_X_LPARAM(lparam);
        auto y = GET_Y_LPARAM(lparam);
        return onMouseDown(x, y);
    }
    case WM_LBUTTONDBLCLK:
    {
        auto x = GET_X_LPARAM(lparam);
        auto y = GET_Y_LPARAM(lparam);
        return onDoubleClick(x,y);
    }
    case WM_LBUTTONUP:
    {
        IsMouseDown = false;
        auto x = GET_X_LPARAM(lparam);
        auto y = GET_Y_LPARAM(lparam);
        return onMouseUp(x, y);
    }
    case WM_MOUSEMOVE:
    {
        auto x = GET_X_LPARAM(lparam);
        auto y = GET_Y_LPARAM(lparam);
        if (IsMouseDown) {
            return onMouseDrag(x, y);
        }
        else {
            return onMouseMove(x, y);
        }        
    }
    case WM_RBUTTONDOWN:
    {
        auto x = GET_X_LPARAM(lparam);
        auto y = GET_Y_LPARAM(lparam);
        onMouseDownRight(x, y);
        return true;
    }
    case WM_KEYDOWN:
    {
        onKeyDown(wparam);
        return false;
    }
    case WM_CHAR: {        
        return onChar(wparam);
    }
    case WM_MOUSEWHEEL: {
        int delta = GET_WHEEL_DELTA_WPARAM(wparam);
        onMouseWheel(delta);
        return false;
    }
    case WM_TIMER: {
        return onTimeout(wparam);
    }
    default:
        break;
    }
    return DefWindowProc(hwnd, msg, wparam, lparam);
}

void WindowMain::paintCanvas()
{
    surfaceBase->writePixels(*pixSrc, 0, 0);
    auto canvas = surfaceBase->getCanvas();
    auto img = surfaceBack->makeImageSnapshot();
    canvas->drawImage(img, 0.f, 0.f);
    img = surfaceFront->makeImageSnapshot();

    canvas->drawImage(img, 0.f, 0.f);
    CutMask::Get()->OnPaint(canvas);
    ToolMain::Get()->OnPaint(canvas);
    ToolSub::Get()->OnPaint(canvas);
}

bool WindowMain::onMouseDown(const int& x, const int& y)
{
    if (ToolMain::Get()->OnMouseDown(x, y)) {
        return true;
    }
    if (ToolSub::Get()->OnMouseDown(x, y)) {
        return true;
    }
    if (CutMask::Get()->OnMouseDown(x, y)) {
        return true;
    }
    if (Recorder::Get()->OnMouseDown(x, y)) {
        return true;
    }
    return false;
}
bool WindowMain::onMouseDownRight(const int& x, const int& y)
{
    bool flag = Recorder::Get()->OnMouseDownRight(x, y);
    if (!flag) {
        App::Quit(2);
    }
    return false;
}
bool WindowMain::onMouseUp(const int& x, const int& y)
{
    if (ToolMain::Get()->OnMouseUp(x, y)) {
        return true;
    }
    if (ToolSub::Get()->OnMouseUp(x, y)) {
        return true;
    }
    Recorder::Get()->OnMouseUp(x, y);
    CutMask::Get()->OnMouseUp(x, y);
    return false;
}
bool WindowMain::onMouseMove(const int& x, const int& y)
{
    auto tm = ToolMain::Get()->OnMouseMove(x, y);
    auto ts = ToolSub::Get()->OnMouseMove(x, y);
    if (tm || ts) {
        return false;
    }
    auto cm = CutMask::Get()->OnMouseMove(x, y);
    if (cm) {
        return false;
    }
    PixelInfo::Get()->OnMouseMove(x, y);
    Recorder::Get()->OnMouseMove(x, y);
    return false;
}
bool WindowMain::onMouseDrag(const int& x, const int& y)
{
    auto tm = ToolMain::Get()->OnMouseMove(x, y);
    auto ts = ToolSub::Get()->OnMouseMove(x, y);
    if (tm || ts) {
        return false;
    }
    if (CutMask::Get()->OnMouseDrag(x, y)) {
        return false;
    }
    Recorder::Get()->OnMouseDrag(x, y);    
    return false;
}
bool WindowMain::onChar(const unsigned int& val)
{
    Recorder::Get()->OnChar(val);
    return false;
}
bool WindowMain::onKeyDown(const unsigned int& val)
{
    bool flag = Recorder::Get()->OnKeyDown(val);
    if (flag) {
        return true;
    }
    flag = CutMask::Get()->OnKeyDown(val);
    if (flag) {
        return true;
    }
    if (val == VK_UP) {
        POINT pos;
        GetCursorPos(&pos);
        SetCursorPos(pos.x, --pos.y);
    }
    else if (val == VK_DOWN) {
        POINT pos;
        GetCursorPos(&pos);
        SetCursorPos(pos.x, ++pos.y);
    }
    else if (val == VK_LEFT) {
        POINT pos;
        GetCursorPos(&pos);
        SetCursorPos(--pos.x, pos.y);
    }
    else if (val == VK_RIGHT) {
        POINT pos;
        GetCursorPos(&pos);
        SetCursorPos(++pos.x, pos.y);
    }
    else if (val == VK_ESCAPE)
    {
        App::Quit(3);
    }
    else if (state > State::mask && GetKeyState(VK_CONTROL) < 0) { 
        if (val == 83) {//Ctrl+S
            App::SaveFile();
        }
        else if (val == 67) {//Ctrl+C
            SaveToClipboard();
        }
        else if (val == 80) { //Ctrl+P
            App::Pin();
        }
        else if (val == 89) { //Ctrl+Y
            Recorder::Get()->Redo();
        }
        else if (val == 90) { //Ctrl+Z
            Recorder::Get()->Undo();
        }
    }
    return true;
}
bool WindowMain::onMouseWheel(const int& delta)
{
    Recorder::Get()->OnMouseWheel(delta);
    return false;
}
bool WindowMain::onDoubleClick(const int& x, const int& y)
{
    if (state >= State::tool && CutMask::GetCutRect().contains(x,y)) {
        SaveToClipboard();
    }
    return false;
}
bool WindowMain::onTimeout(const unsigned int& id)
{
    Recorder::Get()->OnTimeout(id);
    return false;
}
void WindowMain::shotScreen()
{
    HDC hScreen = GetDC(NULL);
    HDC hDC = CreateCompatibleDC(hScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, w, h);
    DeleteObject(SelectObject(hDC, hBitmap));
    BOOL bRet = BitBlt(hDC, 0, 0, w, h, hScreen, x, y, SRCCOPY);
    long long rowBytes = w * 4;
    long long dataSize = rowBytes * h;
    pixSrcData.resize(dataSize);
    BITMAPINFO info = {sizeof(BITMAPINFOHEADER), (long)w, 0 - (long)h, 1, 32, BI_RGB, (DWORD)dataSize, 0, 0, 0, 0};
    GetDIBits(hDC, hBitmap, 0, h, &pixSrcData.front(), &info, DIB_RGB_COLORS);
    DeleteDC(hDC);
    DeleteObject(hBitmap);
    ReleaseDC(NULL, hScreen);
    SkImageInfo imgInfo = SkImageInfo::MakeN32Premul(w, h);
    pixSrc = new SkPixmap(imgInfo, &pixSrcData.front(), rowBytes);
}

void WindowMain::initSize()
{
    x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    h = GetSystemMetrics(SM_CYVIRTUALSCREEN);
}
