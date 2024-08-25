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
    void refresh();
    std::vector<SkColor> winPix;
    std::vector<SkColor> canvasPix;
    std::unique_ptr<SkCanvas> winCanvas;
    std::unique_ptr<SkCanvas> canvas;
    int x, y, w, h;
    bool isMouseDown{false};
    float scaleFactor{ 1.0 };
    HWND hwnd;
    HWND hwndToolTip;
protected:
    virtual void onPaint() = 0;
    virtual void onLeftBtnDown(const int& x, const int& y) {};
    virtual void onLeftBtnUp(const int& x, const int& y) {};
    virtual void onMouseMove(const int& x, const int& y) {};
    virtual void onMouseDrag(const int& x, const int& y) {};
private:
    static LRESULT CALLBACK routeWinMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    void paint();
};
