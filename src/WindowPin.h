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
    static void init();
    static WindowPin* get();
    State state = State::start;
private:
    LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) override;
    void paintTool(SkCanvas* canvas) override;
    bool onMouseDown(const int& x, const int& y);
    bool onMouseDownRight(const int& x, const int& y);
    bool onMouseUp(const int& x, const int& y);
    bool onMouseMove(const int& x, const int& y);
    bool onMouseDrag(const int& x, const int& y);
    bool onChar(const unsigned int& val);
    bool onKeyDown(const unsigned int& val);
    bool onMouseWheel(const int& delta);
    POINT startPos;
};