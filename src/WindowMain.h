#pragma once
#include <Windows.h>
#include <string>
#include <format>
#include "include/core/SkSurface.h"
#include "include/core/SkCanvas.h"

#define WM_REFRESH (WM_APP + 1)

class WindowMain
{
public:
    WindowMain();
    ~WindowMain();

private:
    static LRESULT CALLBACK RouteWindowMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
    HWND hwnd;
    int x{200};
    int y{200};
    int w{1000};
    int h{800};
    HDC hCompatibleDC = NULL;
    HBITMAP bottomHbitmap;
    unsigned char *pixelData;
    sk_sp<SkSurface> surface;
};