#pragma once
#include <Windows.h>
#include <string>
#include <format>
#include "include/core/SkSurface.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkPictureRecorder.h"
#define WM_REFRESH (WM_APP+1)
class WindowBase
{
public:
    WindowBase();
    ~WindowBase();
    void Show();
    void Refresh();
    void Close(const int &exitCode);
    HWND hwnd;
    int x, y, w, h;
    bool IsMouseDown{false};
    sk_sp<SkSurface> surfaceBack;
    sk_sp<SkSurface> surfaceFront;

protected:
    virtual LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) = 0;
    void initWindow();
    void initCanvas();
    virtual void paint(SkCanvas *canvas) = 0;
    unsigned char *pixelBase;
    unsigned char *pixelBack;
    unsigned char *pixelFront;

private:
    static LRESULT CALLBACK RouteWindowMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    HDC hCompatibleDC = NULL;
    HBITMAP bottomHbitmap;
};