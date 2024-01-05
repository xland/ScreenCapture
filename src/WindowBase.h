#pragma once
#include <Windows.h>
#include <string>
#include <format>
#include "include/core/SkSurface.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkPictureRecorder.h"
#include "State.h"

#define WM_REFRESH (WM_APP+1)
class WindowBase
{
public:
    WindowBase();
    virtual ~WindowBase();
    void Show();
    void Refresh();
    void Close(const int &exitCode);
    HWND hwnd;
    State state = State::start;
    int x, y, w, h;
    bool IsMouseDown{false};
    bool IsMouseDragging{ false };
    sk_sp<SkSurface> surfaceBase;
    sk_sp<SkSurface> surfaceBack;
    sk_sp<SkSurface> surfaceFront;
    SkPixmap* pixSrc;
    SkPixmap* pixBase;

protected:
    virtual LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) = 0;
    void initWindow();
    void initCanvas();
    virtual void paintTool(SkCanvas* canvas) = 0;

private:
    void refresh();
    static LRESULT CALLBACK RouteWindowMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    HDC hCompatibleDC = NULL;
    HBITMAP bottomHbitmap;
};