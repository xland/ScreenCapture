#include "WinBase.h"


LRESULT CALLBACK WinBase::routeWinMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_NCCREATE)
    {
        CREATESTRUCT* pCS = reinterpret_cast<CREATESTRUCT*>(lParam);
        LPVOID pThis = pCS->lpCreateParams;
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
    }
    auto obj = reinterpret_cast<WinBase*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    if (obj)
    {
        switch (msg)
        {
        case WM_SETCURSOR:
        {
            return true;
        }
        case WM_PAINT:
        {
            obj->paint();
            break;
        }
        case WM_KEYDOWN:
        {
            switch (wParam)
            {
            case 82: { //R
                if (GetKeyState(VK_CONTROL) < 0)
                {
                    return false;
                }
                [[fallthrough]];
            }
            case 72: { //H
                if (GetKeyState(VK_CONTROL) < 0)
                {
                    return false;
                }
                [[fallthrough]];
            }
            default: {
                break;
            }
            }
            [[fallthrough]];
        }
        default:
        {
            return obj->wndProc(hWnd, msg, wParam, lParam);
        }
        }
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}
