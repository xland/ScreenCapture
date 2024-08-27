#pragma once
#include <Windows.h>
#include <vector>
#include <functional>
#include "include/core/SkColor.h"
#include "include/core/SkCanvas.h"
#include "TypeDefine.h"


class WinBase
{
public:
    WinBase();
    ~WinBase();
    virtual void Init()=0;
    void Refresh();
    void UpdateState(const State& state);
    void Emit(const EventType& et,const uint32_t& msg=0);

    std::vector<MouseEventCB> mouseMoveHandlers;
    std::vector<MouseEventCB> mouseDragHandlers;
    std::vector<MouseEventCB> leftBtnDownHandlers;
    std::vector<MouseEventCB> leftBtnUpHandlers;
    std::vector<CustomEventCB> customEventHandlers;
    std::vector<PaintEventCB> paintHandlers;

    State state{ State::start };
    State statePre{ State::start };
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
    void InitWindow();
    void InitSurface();
    virtual void onPaint() = 0;
    void onLeftBtnDown(const int& x, const int& y);
    void onLeftBtnUp(const int& x, const int& y);
    void onMouseMove(const int& x, const int& y);
    void onMouseDrag(const int& x, const int& y);
    void onCustomMsg(const EventType& type, const uint32_t& msg);
private:
    static LRESULT CALLBACK routeWinMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    void paint();
};
