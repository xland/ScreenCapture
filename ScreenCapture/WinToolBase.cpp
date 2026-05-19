#include "pch.h"
#include "WinToolBase.h"
#include "WinCap.h"
#include "App.h"
#include "Util.h"

WinToolBase::WinToolBase()
{}

WinToolBase::~WinToolBase()
{}

void WinToolBase::createWindow(int btnCount)
{
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = &WinToolBase::winMsg;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = GetModuleHandle(NULL);
    wcex.hIcon = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_WINLOGO);
    wcex.hCursor = LoadCursor(nullptr, IDC_HAND);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"WinTool";
    wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_WINLOGO);
    RegisterClassEx(&wcex);
    auto win = WinCap::get();
    btnW = btnW * win->dpi;
    btnH = btnH * win->dpi;
    btnPadding = 3.f * win->dpi;
    w = btnW * btnCount;
    h = btnH;
    hwnd = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_TOPMOST, wcex.lpszClassName, NULL, WS_POPUP, 0, 0, w, h, NULL, NULL, wcex.hInstance, NULL);
    SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    dpi = GetDpiForWindow(hwnd) / 96.0f;

    render = Util::createRender(hwnd, w, h);
    render->CreateSolidColorBrush(D2D1::ColorF(0xe6f4ff), brushBg.GetAddressOf());
    render->CreateSolidColorBrush(D2D1::ColorF(0x333333), brushIcon.GetAddressOf());
    render->CreateSolidColorBrush(D2D1::ColorF(0x1677ff), brushIconHover.GetAddressOf());
    render->CreateSolidColorBrush(D2D1::ColorF(0xbbbbbb), brushSpliter.GetAddressOf());
}
void WinToolBase::hide()
{
    auto win = WinCap::get();
    //win->changeDrawState(L"");
    ShowWindow(hwnd, SW_HIDE);
}
void WinToolBase::close()
{
    SetWindowLongPtr(hwnd, GWLP_USERDATA, 0);
    DestroyWindow(hwnd);
}
void WinToolBase::onMouseLeave()
{
    Util::trackMouse(hwnd, true);
    mouseIn = false;
    hoverIndex = -1;
    InvalidateRect(hwnd, nullptr, false);
    auto win = WinCap::get();
    SetFocus(win->hwnd);
}

void WinToolBase::paint()
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);
    render->BeginDraw();
    render->Clear(D2D1::ColorF(0xFFFFFF));
	onPaint();
    render->EndDraw();
    EndPaint(hwnd, &ps);
}

void WinToolBase::onMouseMove(const int& x, const int& y)
{
    if (!mouseIn)
    {
        mouseIn = true;
        Util::trackMouse(hwnd);
    }
    auto index = static_cast<int>(x / btnW);
    if (index != hoverIndex) {
        hoverIndex = index;
        InvalidateRect(hwnd, nullptr, false);
    }
}
void WinToolBase::onKeyDown(const TCHAR& key)
{
    bool ctrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
    if (!ctrl) return;
    if (key == L'Z') {
        //WinCap::get()->undo();
    }
    else if (key == L'Y') {
        //WinCap::get()->redo();
    }
}

LRESULT WinToolBase::winMsg(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    auto self = reinterpret_cast<WinToolBase*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    if (!self) {
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    else if (msg == WM_RBUTTONDOWN)
    {
        App::exit(2);
    }
    else if (msg == WM_ERASEBKGND)
    {
        return 1;
    }
    else if (msg == WM_PAINT)
    {
        self->paint();
    }
    else if (msg == WM_LBUTTONDOWN) {
        self->onClick();
    }
    else if (msg == WM_MOUSEMOVE) {
        self->onMouseMove(LOWORD(lParam), HIWORD(lParam));
    }
    else if (msg == WM_MOUSELEAVE) {
        self->onMouseLeave();
    }
    else if (msg == WM_KEYDOWN) {
        self->onKeyDown((TCHAR)wParam);
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}