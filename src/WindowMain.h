#pragma once
#include <Windows.h>
#include <string>
#include <format>
#include "WindowBase.h"

class WindowMain : public WindowBase
{
public:
    WindowMain();
    ~WindowMain();

private:
    LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) override;
    void shotScreen();
    void initSize();
    void paint(SkCanvas *canvas) override;
    unsigned char *desktopPixel;
};