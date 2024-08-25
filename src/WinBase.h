#pragma once
#include <Windows.h>
#include <vector>
#include "include/core/SkColor.h"
#include "include/core/SkCanvas.h"

class WinBase
{
public:
    WinBase();
    ~WinBase();
    void initWindow();
    void initSurface();
    std::vector<SkColor> winPix;
    std::vector<SkColor> canvasPix;
    std::unique_ptr<SkCanvas> winCanvas;
    std::unique_ptr<SkCanvas> canvas;
    int x, y, w, h;
    bool isMouseDown;
    HWND hwnd;
    HWND hwndToolTip;
protected:
    virtual LRESULT CALLBACK wndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) = 0;
    virtual void onPaint() = 0;
    void onLeftBtnDown(const int& x, const int& y) {};
    void onLeftBtnUp(const int& x, const int& y) {};
    void onMouseMove(const int& x, const int& y) {};
private:
    static LRESULT CALLBACK routeWinMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    void paint();
};
