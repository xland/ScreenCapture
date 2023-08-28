﻿#include "WindowBase.h"
#include <windowsx.h>
#include <dwmapi.h>
#include "Util.h"
#include <format>
#include "MainWin.h"
#include "PinWin.h"
#include <thread>

/// <summary>
/// 0 undefined
/// 1 quit by press close btn;
/// 2 quit by press right mouse btn;
/// 3 quit by press esc keyboard
/// 4 quit when copy rgb color
/// 5 quit when copy hex color
/// 6 quit when save to file
/// 7 quit when save to clipboard
/// </summary>
static int mainWinQuitCode = 0;
static WindowBase* windowInstance;
LRESULT CALLBACK WindowBase::RouteWindowMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_NCCREATE)
    {
        CREATESTRUCT* pCS = reinterpret_cast<CREATESTRUCT*>(lParam);
        LPVOID pThis = pCS->lpCreateParams;
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
    }
    auto obj = reinterpret_cast<WindowBase*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    if (obj) {
        if (msg == WM_CLOSE) {
            if (obj->IsMainWin) {
                delete (MainWin*)obj;
            }
            else
            {
                delete (PinWin*)obj;
            }            
        }
        else
        {
            return obj->WindowProc(hWnd, msg, wParam, lParam);
        }
        
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}


WindowBase::WindowBase(){
    windowInstance = this;
}

WindowBase::~WindowBase()
{    
    delete PaintCtx;
    delete OriginalImage;
    delete CanvasImage;
    delete PrepareImage;
    delete MosaicImage;
    delete BottomImage;
    DeleteObject(hCompatibleDC);
    DeleteObject(bottomHbitmap);
}

WindowBase* WindowBase::Get()
{
    return windowInstance;
}

void WindowBase::quitApp(const int& exitCode)
{
    mainWinQuitCode = exitCode;
    SendMessage(hwnd, WM_CLOSE, NULL, NULL);
    PostQuitMessage(0);
}
int WindowBase::GetQuitCode()
{
    return mainWinQuitCode;
}

void WindowBase::InitWindow()
{
    static int num = 0;
    std::wstring className = std::format(L"ScreenCapture{}", num++);
    auto hinstance = GetModuleHandle(NULL);
    WNDCLASSEX wcx{};
    wcx.cbSize = sizeof(wcx);
    wcx.style = CS_HREDRAW | CS_VREDRAW;
    wcx.lpfnWndProc = &WindowBase::RouteWindowMessage;
    wcx.cbWndExtra = sizeof(WindowBase*);
    wcx.hInstance = hinstance;
    wcx.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcx.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcx.lpszClassName = className.c_str();
    if (!RegisterClassEx(&wcx))
    {
        MessageBox(NULL, L"注册窗口类失败", L"系统提示", NULL);
        return;
    }
    this->hwnd = CreateWindowEx(WS_EX_LAYERED, wcx.lpszClassName, wcx.lpszClassName, WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_POPUP, x, y, w, h, NULL, NULL, hinstance, static_cast<LPVOID>(this));
    BOOL attrib = TRUE;
    DwmSetWindowAttribute(hwnd, DWMWA_TRANSITIONS_FORCEDISABLED, &attrib, sizeof(attrib));//移除窗口打开与关闭时的动画效果
    SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);    
}

void WindowBase::Show()
{
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
    ChangeCursor(IDC_ARROW);
    Refresh();
}

void WindowBase::Close()
{
    SendMessage(hwnd, WM_CLOSE, NULL, NULL);
}

bool WindowBase::OnTimer(const unsigned int& id)
{
    if (id == 999) {
        auto shape = dynamic_cast<Shape::Text*>(History::GetLastDrawShape());
        if (shape) {
            shape->Draw(-1, -1, -1, -1);
        }
    }
    else if (id == 998) {
        saveClipboard();
    }
    return true;
}

void WindowBase::InitLayerImg() {
    PrepareImage = new BLImage(w, h, BL_FORMAT_PRGB32);
    CanvasImage = new BLImage(w, h, BL_FORMAT_PRGB32);
    BottomImage = new BLImage(w, h, BL_FORMAT_PRGB32);
    PaintCtx = new BLContext();
    PaintCtx->begin(*PrepareImage);
    PaintCtx->clearAll();
    PaintCtx->end();
    PaintCtx->begin(*CanvasImage);
    PaintCtx->clearAll();
    PaintCtx->end();
    BLImageData imgData;
    BottomImage->getData(&imgData);
    stride = imgData.stride;
    pixelData = (unsigned char*)imgData.pixelData;
}
void WindowBase::Refresh()
{
    PostMessage(hwnd, WM_REFRESH, NULL, NULL);
    //InvalidateRect(hwnd, nullptr, false);
}

LRESULT CALLBACK WindowBase::WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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
        case WM_NCHITTEST:
        {
            return OnHitTest(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        }
        case WM_SETCURSOR: {
            return true;
        }
        case WM_REFRESH: {
            BeforePaint();
            HDC hdc;
            hdc = GetDC(hwnd);
            if (hCompatibleDC == NULL)
            {
                hCompatibleDC = CreateCompatibleDC(NULL);
                bottomHbitmap = CreateCompatibleBitmap(hdc, w, h); //创建一副与当前DC兼容的位图
                SelectObject(hCompatibleDC, bottomHbitmap);
            }
            BITMAPINFO info = { sizeof(BITMAPINFOHEADER), (long)w, 0 - (long)h, 1, 32, BI_RGB, stride*h, 0, 0, 0, 0 };
            SetDIBits(hdc, bottomHbitmap, 0, h, pixelData, &info, DIB_RGB_COLORS); //使用指定的DIB颜色数据来设置位图中的像素
            BLENDFUNCTION blend = { 0 };
            blend.BlendOp = AC_SRC_OVER;
            blend.SourceConstantAlpha = 255;
            blend.AlphaFormat = AC_SRC_ALPHA;//按通道混合
            POINT	pSrc = { 0, 0 };
            SIZE	sizeWnd = { w, h };
            UpdateLayeredWindow(hwnd, hdc, NULL, &sizeWnd, hCompatibleDC, &pSrc, NULL, &blend, ULW_ALPHA);//更新分层窗口
            ReleaseDC(hwnd, hdc);
            return 1;
        }
        case WM_TIMER: {
            return OnTimer(wParam);            
        }
        case WM_RBUTTONDOWN: {
            return OnRightButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));            
        }
        case WM_LBUTTONDOWN: {
            IsLeftBtnDown = true;
            return OnLeftButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));            
        }
        case WM_LBUTTONUP: {
            IsLeftBtnDown = false;
            return OnLeftButtonUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        }
        case WM_MOUSEMOVE: {
            return OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));            
        }
        case WM_KEYDOWN: {
            return OnKeyDown(wParam);
        }
        case WM_KEYUP: {
            return OnKeyUp(wParam);
        }
        case WM_CHAR: {
            return OnChar(wParam);
        }
        case WM_SIZE:
        {
/*            if (painter) {
                this->w = LOWORD(lParam);
                this->h = HIWORD(lParam);
                OnResize();
                painter->OnResize(hWnd, w, h);
            }   */        
            this->w = LOWORD(lParam);
            this->h = HIWORD(lParam);
            OnResize();
            return true;
        }
        case WM_EXITSIZEMOVE: {
            RECT rect;
            GetWindowRect(hWnd, &rect);
            this->x = rect.left;
            this->y = rect.top;
            return true;
        }
        //case WM_WINDOWPOSCHANGING: {

        //}
    }    
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}