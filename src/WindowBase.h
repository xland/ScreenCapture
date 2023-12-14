#pragma once
#include <Windows.h>
#include <string>
#include <format>
#include "include/core/SkSurface.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkPictureRecorder.h"

class WindowBase
{
public:
    WindowBase();
    ~WindowBase();
    void Show();
    void Refresh();
    void Close(const int &exitCode);
    int x, y, w, h;
    bool IsMouseDown{false};

protected:
    virtual LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) = 0;
    void initWindow();
    void initCanvas();
    virtual void paint(SkCanvas *base, SkCanvas *board, SkCanvas *canvas) = 0;
    virtual void paintFinish(SkCanvas *base) = 0;
    unsigned char *pixelBase;
    unsigned char *pixelBoard;
    unsigned char *pixelCanvas;

private:
    static LRESULT CALLBACK RouteWindowMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    HWND hwnd;
    HDC hCompatibleDC = NULL;
    HBITMAP bottomHbitmap;
    sk_sp<SkSurface> surfaceBase;
    sk_sp<SkSurface> surfaceBoard;
    sk_sp<SkSurface> surfaceCanvas;
};