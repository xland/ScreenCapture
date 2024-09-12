#include "WinBase.h"
#include <windowsx.h>

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
            case WM_LBUTTONDOWN:
            {
                obj->onLeftBtnDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                break;
            }
            case WM_LBUTTONUP:
            {
                obj->onLeftBtnUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                break;
            }
            case WM_MOUSEMOVE:
            {
                obj->onMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                break;
            }
            case CustomMsgId: {
                obj->onCustomMsg((EventType)wParam,(uint32_t)lParam);
                break;
            }
            case WM_TIMER: {
                if (wParam == RefreshTimerId) {
                    KillTimer(hWnd, RefreshTimerId);
                    obj->refreshFlag = false;
                    InvalidateRect(hWnd, nullptr, false);
                }
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
                break;
            }
        }
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}
