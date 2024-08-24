#include "WinBase.h"
#include <format>
#include <Windows.h>
#include "App.h"
#include "include/core/SkColor.h"
#include "include/core/SkImageInfo.h"
#include "include/core/SkCanvas.h"

WinBase::WinBase()
{
}

WinBase::~WinBase()
{
}

void WinBase::paint()
{
    SkColor* surfaceMemory = new SkColor[w * h]{ SK_ColorBLACK };
    SkImageInfo info = SkImageInfo::MakeN32Premul(w, h);
    std::unique_ptr<SkCanvas> canvas = SkCanvas::MakeRasterDirect(info, surfaceMemory, w * sizeof(SkColor));
    SkPaint paint;
    paint.setColor(SK_ColorRED);
    SkRect rect = SkRect::MakeXYWH(w - 150, h - 150, 140, 140);
    canvas->drawRect(rect, paint);

    PAINTSTRUCT ps;
    auto dc = BeginPaint(hwnd, &ps);
    BITMAPINFO bmpInfo = { sizeof(BITMAPINFOHEADER), w, -h, 1, 32, BI_RGB, h * 4 * w, 0, 0, 0, 0 };
    StretchDIBits(dc, 0, 0, w, h, 0, 0, w, h, surfaceMemory, &bmpInfo, DIB_RGB_COLORS, SRCCOPY);
    ReleaseDC(hwnd, dc);
    EndPaint(hwnd, &ps);
    delete[] surfaceMemory;
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

