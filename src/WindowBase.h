#pragma once
#include <Windows.h>
#include <string>
#include <format>
#include "include/core/SkSurface.h"
#include "include/core/SkCanvas.h"

class WindowBase
{
public:
    WindowBase();
    ~WindowBase();
    void Show();
    void Refresh();

protected:
    virtual LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) = 0;
    void initWindow();
    void initCanvas();
    virtual void paint(SkCanvas* canvas) = 0;
    int x, y, w, h;
    unsigned char *pixelData;

private:
    static LRESULT CALLBACK RouteWindowMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    HWND hwnd;
    HDC hCompatibleDC = NULL;
    HBITMAP bottomHbitmap;
    sk_sp<SkSurface> surface;
};