#include "WinBase.h"
#include <format>
#include <Windows.h>
#include "App.h"
#include "include/core/SkColor.h"
#include "include/core/SkImageInfo.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkSurface.h"

WinBase::WinBase()
{
}

WinBase::~WinBase()
{
}

void WinBase::paint()
{
    onPaint();
    PAINTSTRUCT ps;
    auto dc = BeginPaint(hwnd, &ps);
    BITMAPINFO bmpInfo = { sizeof(BITMAPINFOHEADER), w, -h, 1, 32, BI_RGB, h * 4 * w, 0, 0, 0, 0 };
    StretchDIBits(dc, 0, 0, w, h, 0, 0, w, h, &winPix.front(), &bmpInfo, DIB_RGB_COLORS, SRCCOPY);
    ReleaseDC(hwnd, dc);
    EndPaint(hwnd, &ps);
}

void WinBase::initWindow()
{
    static int num = 0;
    std::wstring clsName = std::format(L"ScreenCapture{}", num++);
    auto hinstance = App::Get()->instance;
    WNDCLASSEX wcx{};
    wcx.cbSize = sizeof(wcx);
    wcx.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wcx.lpfnWndProc = &WinBase::routeWinMsg;
    wcx.cbWndExtra = sizeof(WinBase*);
    wcx.hInstance = hinstance;
    wcx.hIcon = LoadIcon(hinstance, IDI_APPLICATION);
    wcx.hCursor = LoadCursor(hinstance, IDC_ARROW);
    wcx.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcx.lpszClassName = clsName.c_str();
    RegisterClassEx(&wcx);
    this->hwnd = CreateWindowEx(0, clsName.c_str(), clsName.c_str(),
        WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_POPUP, 
        x, y, w, h, NULL, NULL, hinstance, static_cast<LPVOID>(this));    
    ShowWindow(hwnd, SW_SHOW);
}

void WinBase::initSurface()
{
    auto dataSize = h * w;
    winPix.resize(dataSize);
    canvasPix.resize(dataSize, 0x00000000);
    SkImageInfo info = SkImageInfo::MakeN32Premul(w, h);
    auto rowSize = w * sizeof(SkColor);
    winCanvas = SkCanvas::MakeRasterDirect(info, &winPix.front(), rowSize);
    canvas = SkCanvas::MakeRasterDirect(info, &canvasPix.front(), rowSize);
}

