#include "pch.h"
#include "Util.h"
#include "WinTool.h"
#include "App.h"
#include "WinCap.h"
#include "CutMask.h"
//#include "WinPin.h"


WinTool::WinTool()
{
    createWindow();
    initRender();
}
WinTool::~WinTool()
{
    //brushBg.Reset();
    //brushIcon.Reset();
    //brushIconHover.Reset();
    //brushSpliter.Reset();
    //render.Reset();
}
void WinTool::createWindow()
{
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = &WinTool::winMsg;
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
    auto app = App::get();
    auto count = std::count_if(app->toolBtns.begin(), app->toolBtns.end(), [](auto& s) { return s != L"|"; });
    w = btnW * count;
    h = btnH;
    hwnd = CreateWindowEx(WS_EX_TOOLWINDOW|WS_EX_TOPMOST,wcex.lpszClassName, NULL, WS_POPUP, 0, 0, w, h, NULL, NULL, wcex.hInstance, NULL);
    SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    dpi = GetDpiForWindow(hwnd) / 96.0f;
}

void WinTool::initRender()
{
    render = Util::createRender(hwnd, w, h);
    render->CreateSolidColorBrush(D2D1::ColorF(0xe6f4ff), brushBg.GetAddressOf());
    render->CreateSolidColorBrush(D2D1::ColorF(0x333333), brushIcon.GetAddressOf());
    render->CreateSolidColorBrush(D2D1::ColorF(0x1677ff), brushIconHover.GetAddressOf());
    render->CreateSolidColorBrush(D2D1::ColorF(0xbbbbbb), brushSpliter.GetAddressOf());
}

IDWriteTextLayout* WinTool::getIconLayout(const std::wstring& icon)
{
    static std::unordered_map<std::wstring, ComPtr<IDWriteTextLayout>> btnIcons;
    if (!btnIcons.contains(icon)) {
        auto fontSize{ 14.f * dpi };
        btnIcons.clear();
        btnIcons.insert({ L"rect" ,Util::getIconLayout(L"\ue8e8",fontSize,btnW,h) });
        btnIcons.insert({ L"ellipse" ,Util::getIconLayout(L"\ue6bc",fontSize,btnW,h) });
        btnIcons.insert({ L"arrow" ,Util::getIconLayout(L"\ue603",fontSize,btnW,h) });
        btnIcons.insert({ L"number" ,Util::getIconLayout(L"\ue776",fontSize,btnW,h) });
        btnIcons.insert({ L"line" ,Util::getIconLayout(L"\ue601",fontSize,btnW,h) });
        btnIcons.insert({ L"text" ,Util::getIconLayout(L"\ue6ec",fontSize,btnW,h) });
        btnIcons.insert({ L"mosaic" ,Util::getIconLayout(L"\ue82e",fontSize,btnW,h) });
        btnIcons.insert({ L"eraser" ,Util::getIconLayout(L"\ue6be",fontSize,btnW,h) });
        btnIcons.insert({ L"undo" ,Util::getIconLayout(L"\ued85",fontSize,btnW,h) });
        btnIcons.insert({ L"redo" ,Util::getIconLayout(L"\ued8a",fontSize,btnW,h) });
        btnIcons.insert({ L"pin" ,Util::getIconLayout(L"\ue6a2",fontSize,btnW,h) });
        btnIcons.insert({ L"clipboard" ,Util::getIconLayout(L"\ue650",fontSize,btnW,h) });
        btnIcons.insert({ L"save" ,Util::getIconLayout(L"\ue608",fontSize,btnW,h) });
        btnIcons.insert({ L"close" ,Util::getIconLayout(L"\ue62d",fontSize,btnW,h) });
    }
    return btnIcons[icon].Get();
}

void WinTool::onMouseLeave()
{
    Util::trackMouse(hwnd,true);
    mouseIn = false;
    hoverIndex = -1;
    InvalidateRect(hwnd, nullptr, false);
    auto win = WinCap::get();
    SetFocus(win->hwnd);
}

void WinTool::onPaint()
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);
    render->BeginDraw();
    render->Clear(D2D1::ColorF(0xFFFFFF));
    auto app = App::get();
    int btnIndex{ 0 };
    auto win = WinCap::get();
    for (auto& btn : app->toolBtns)
    {
        if (btn == L"|") {
            auto lx{ btnW * btnIndex }, lt{ 3.f * btnPadding };
            render->DrawLine({ lx,lt }, { lx,btnH - lt }, brushSpliter.Get(), 0.8f);
        }
        else
        {           
            if (btnIndex == hoverIndex || btn == win->drawState) {
                auto xStart{ btnW * btnIndex };
                D2D1_ROUNDED_RECT rr = { { xStart + btnPadding, btnPadding, xStart + btnW - btnPadding, h - btnPadding }, 6, 6 };
                render->FillRoundedRectangle(rr, brushBg.Get());
                D2D1_POINT_2F origin = { xStart, 0.f };
                render->DrawTextLayout(origin, getIconLayout(btn), brushIconHover.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
            }
            else {
                D2D1_POINT_2F origin = { btnW * btnIndex, 0.f };
                render->DrawTextLayout(origin, getIconLayout(btn), brushIcon.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
            }
            btnIndex += 1;
        }
    }
    render->EndDraw();
    EndPaint(hwnd, &ps);
}

void WinTool::onMouseMove(const int& x, const int& y)
{
    if (!mouseIn)
    {        
        mouseIn = true;
        Util::trackMouse(hwnd);
    }
    auto index = static_cast<int>(x/btnW);
    if (index != hoverIndex) {
        hoverIndex = index;
        InvalidateRect(hwnd, nullptr, false);
    }
}

void WinTool::onMouseClick()
{
    auto index{ 0 };
    std::wstring* btnName{nullptr};
    for (auto& btn: App::get()->toolBtns)
    {
        if (btn == L"|") continue;
        btnName = &btn;
        if (index == hoverIndex) break;
        index += 1;
    }
    if (!btnName) return;
    if (*btnName == L"close") {
        //App::exit(2);
    }
    else if (*btnName == L"pin") {
        //WinPin::create();
    }
    else if (*btnName == L"save") {
        //WinCap::get()->saveToDisk();
    }
    else if (*btnName == L"clipboard") {
        //WinCap::get()->saveToClipboard();
        //App::exit(2);
    }
    else if (*btnName == L"rect") {
        //WinCap::get()->changeDrawState(L"rect");
        InvalidateRect(hwnd, nullptr, false);
    }
    else if (*btnName == L"arrow") {
        //WinCap::get()->changeDrawState(L"arrow");
        InvalidateRect(hwnd, nullptr, false);
    }
    else if (*btnName == L"more") {
        //Env::start();
        //WinCap::stop();
    }
    else if (*btnName == L"video") {

    }
    else if (*btnName == L"long") {

    }
}

void WinTool::changeSelectBtn(const std::wstring& name)
{
    auto win = WinCap::get();
    if (win->drawState == name) {
        win->drawState = std::wstring{};
    }
    else {
        win->drawState = name;
    }
    InvalidateRect(hwnd, nullptr, false);
}

void WinTool::onKeyDown(const TCHAR& key)
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

LRESULT WinTool::winMsg(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    auto self = reinterpret_cast<WinTool*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    if (!self) {
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    else if (msg == WM_RBUTTONDOWN)
    {
        //App::exit(2);
    }
    else if (msg == WM_ERASEBKGND)
    {
        return 1;
    }
    else if (msg == WM_PAINT)
    {
        self->onPaint();
    }
    else if (msg == WM_LBUTTONDOWN) {
        self->onMouseClick();
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

void WinTool::hide()
{
    auto win = WinCap::get();
    //win->changeDrawState(L"");
    ShowWindow(hwnd, SW_HIDE);
}

void WinTool::show()
{
    auto win = WinCap::get();
    //win->changeDrawState(L"rect");
    x = static_cast<int>(win->cutMask->maskRect.right - w);
    y = static_cast<int>(win->cutMask->maskRect.bottom + 7 * win->dpi);
    if (y + h > win->h) {
        y = win->cutMask->maskRect.top - 7 * win->dpi - h;
    }
    if (y < win->y) {
        y = win->cutMask->maskRect.bottom - 7 * win->dpi - h;
        x -= 7 * win->dpi;
    }
    SetWindowPos(hwnd, nullptr,x,y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
    InvalidateRect(hwnd, nullptr, false);
    ShowWindow(hwnd, SW_SHOW);
}

void WinTool::close()
{
    SetWindowLongPtr(hwnd, GWLP_USERDATA, 0);
    DestroyWindow(hwnd);
}
