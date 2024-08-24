#pragma once
#include <Windows.h>

class WinBase
{
public:
    WinBase();
    ~WinBase();
    void initWindow();
    int x, y, w, h;
    HWND hwnd;
    HWND hwndToolTip;
protected:
    virtual LRESULT CALLBACK wndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) = 0;
private:
    static LRESULT CALLBACK routeWinMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    void paint();
};
