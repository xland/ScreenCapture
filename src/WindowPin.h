#pragma once
#include <Windows.h>
#include <string>
#include <format>
#include "WindowBase.h"
#include "State.h"

class WindowPin : public WindowBase
{
public:
    WindowPin();
    ~WindowPin();
private:
    virtual void initCanvas();
    void initSize();
    void showMenu();
    LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) override;
    void paintCanvas() override;
    bool onMouseDown(const int& x, const int& y);
    bool onMouseDownRight(const int& x, const int& y);
    bool onMouseUp(const int& x, const int& y);
    bool onMouseMove(const int& x, const int& y);
    bool onMouseDrag(const int& x, const int& y);
    bool onChar(const unsigned int& val);
    bool onKeyDown(const unsigned int& val);
    bool onMouseWheel(const int& delta);
    POINT startPos;
    float shadowSize{ 8.0f };
};