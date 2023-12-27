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
    //void paint(SkCanvas *canvas) override;
    //void paintFinish(SkCanvas* canvas) override;
    bool onMouseDown(const int& x, const int& y);
    bool onMouseUp(const int& x, const int& y);
    bool onMouseMove(const int& x, const int& y);
    bool onMouseDrag(const int& x, const int& y);
    bool onChar(const unsigned int& val);
    bool onKeyDown(const unsigned int& val);
};