#pragma once
#include <Windows.h>
#include <string>
#include <format>
#include "WindowBase.h"

class CutMask;
class WindowMain : public WindowBase
{
public:
    ~WindowMain();
    static void init();
    static WindowMain *get();

private:
    WindowMain();
    LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) override;
    void shotScreen();
    void initSize();
    void paint(SkCanvas *base, SkCanvas *board, SkCanvas *canvas) override;
    void paintFinish(SkCanvas *base) override;
    bool onMouseDown(int x, int y);
    bool onMouseUp(int x, int y);
    bool onMouseMove(int x, int y);
    unsigned char *desktopPixel;
    CutMask *cutMask;
};