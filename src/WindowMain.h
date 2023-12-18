#pragma once
#include <Windows.h>
#include <string>
#include <format>
#include "WindowBase.h"
#include "State.h"

class CutMask;
class WindowMain : public WindowBase
{
public:
    ~WindowMain();
    static void init();
    static WindowMain *get();
    State state = State::start;
private:
    WindowMain();
    LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) override;
    void shotScreen();
    void initSize();
    void paint(SkCanvas *canvas) override;
    bool onMouseDown(int x, int y);
    bool onMouseUp(int x, int y);
    bool onMouseMove(int x, int y);
    bool onMouseDrag(int x, int y);
};