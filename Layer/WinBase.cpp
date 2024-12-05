
#include <dwmapi.h>
#include <sstream>
#include <string>

#include "WinBase.h"


WinBase::WinBase()
{
}

WinBase::~WinBase()
{
}

void WinBase::Show()
{
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
    Refresh();
}

void WinBase::Refresh()
{
    PostMessage(hwnd, WM_REFRESH, NULL, NULL);
}

void WinBase::Close(const int &exitCode)
{
    SendMessage(hwnd, WM_CLOSE, NULL, NULL);    
}

void WinBase::refresh()
{
    paint(canvas.get());

    HDC hdc = GetDC(hwnd);
    auto hCompatibleDC = CreateCompatibleDC(NULL);
    auto bottomHbitmap = CreateCompatibleBitmap(hdc, w, h);
    DeleteObject(SelectObject(hCompatibleDC, bottomHbitmap));
    HDC hdc = GetDC(hwnd);
    BITMAPINFO info = { sizeof(BITMAPINFOHEADER), w, 0 - h, 1, 32, BI_RGB, w * 4 * h, 0, 0, 0, 0 };
    SetDIBits(hdc, bottomHbitmap, 0, h, &pix.front(), &info, DIB_RGB_COLORS);
    BLENDFUNCTION blend = { .BlendOp{AC_SRC_OVER}, .SourceConstantAlpha{255}, .AlphaFormat{AC_SRC_ALPHA} };
    POINT pSrc = { 0, 0 };
    SIZE sizeWnd = { w, h };
    UpdateLayeredWindow(hwnd, hdc, NULL, &sizeWnd, hCompatibleDC, &pSrc, NULL, &blend, ULW_ALPHA);
    ReleaseDC(hwnd, hdc);
}

void WinBase::SetTimeout(const unsigned int& id,const unsigned int& ms)
{
    SetTimer(hwnd, id, ms, (TIMERPROC)NULL);
}

void WinBase::ClearTimeout(const unsigned int& id)
{
    KillTimer(hwnd, id);
}

LRESULT CALLBACK WinBase::RouteWindowMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_NCCREATE)
    {
        CREATESTRUCT *pCS = reinterpret_cast<CREATESTRUCT *>(lParam);
        LPVOID pThis = pCS->lpCreateParams;
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
    }
    auto obj = reinterpret_cast<WinBase*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    if (obj)
    {
        switch (msg)
        {
        case WM_NCCALCSIZE:
        {
            if (wParam == TRUE)
            {
                return false;
            }
            break;
        }
        case WM_REFRESH: {
            obj->refresh();
            return true;
        }
        case WM_SETCURSOR:
        {
            return true;
        }
        case WM_KEYDOWN:
        {
            switch (wParam)
            {
            case 82: { //R
                if (GetKeyState(VK_CONTROL) < 0)
                {
                    //obj->setClipboardText(obj->getPixelRgb());
                    //App::Quit(4);
                    return false;
                }
                [[fallthrough]];
            }
            case 72: { //H
                if (GetKeyState(VK_CONTROL) < 0)
                {
                    //obj->setClipboardText(obj->getPixelHex());
                    //App::Quit(5);
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
void WinBase::initWindow()
{
    static int num = 0;
    std::wstring className = std::format(L"ScreenCapture{}", num++);
    auto hinstance = GetModuleHandle(NULL);
    WNDCLASSEX wcx{};
    wcx.cbSize = sizeof(wcx);
    wcx.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wcx.lpfnWndProc = &WinBase::RouteWindowMessage;
    wcx.cbWndExtra = sizeof(WinBase*);
    wcx.hInstance = hinstance;
    wcx.hIcon = LoadIcon(hinstance, IDI_APPLICATION);
    wcx.hCursor = LoadCursor(hinstance, IDC_ARROW);
    wcx.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcx.lpszClassName = className.c_str();
    if (!RegisterClassEx(&wcx))
    {
        return;
    }
#ifdef _DEBUG
    auto exStyle = className.ends_with(L'0') ? WS_EX_LAYERED : WS_EX_LAYERED | WS_EX_TOPMOST;
#else
    auto exStyle = WS_EX_LAYERED | WS_EX_TOPMOST;
#endif
    this->hwnd = CreateWindowEx(exStyle, className.c_str(), className.c_str(), WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_POPUP,
                                x, y, w, h, NULL, NULL, hinstance, static_cast<LPVOID>(this));

    hwndTip = CreateWindowEx(WS_EX_TOPMOST, TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hwnd, NULL, hinstance, NULL);
}



std::wstring WinBase::getPixelRgb()
{
    //POINT pos;
    //GetCursorPos(&pos);
    //ScreenToClient(hwnd, &pos); //钉图
    //auto color = pix->getColor4f(pos.x, pos.y);
    //int R{ (int)(color.fR * 255) }, G{ (int)(color.fG * 255) }, B{ (int)(color.fB * 255) };
    //return std::format(L"{},{},{}", std::to_wstring(R), std::to_wstring(G), std::to_wstring(B));
}
std::wstring WinBase::getPixelHex()
{
    //POINT pos;
    //GetCursorPos(&pos);
    //ScreenToClient(hwnd, &pos); //钉图
    //auto color = pixBase->getColor4f(pos.x, pos.y);
    //int R{ (int)(color.fR * 255) }, G{ (int)(color.fG * 255) }, B{ (int)(color.fB * 255) };
    //std::wstringstream ss;
    //ss << std::hex << (R << 16 | G << 8 | B);
    //std::wstring hex = ss.str();
    //size_t str_length = hex.length();
    //for (size_t i = 0; i < 6 - str_length; i++) {
    //    hex = L"0" + hex;
    //}
    //std::transform(hex.begin(), hex.end(), hex.begin(), toupper);
    //return std::format(L"#{}", hex);
}