#include "WinBase.h"
#include <format>
#include <Windows.h>
#include "App.h"
#include "include/core/SkColor.h"
#include "include/core/SkImageInfo.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkSurface.h"
#include "Tool/ToolMain.h"
#include "Tool/ToolSub.h"

WinBase::WinBase()
{
}

WinBase::~WinBase()
{
}

void WinBase::Init()
{
    initTool();
    initSurface();
    initWindow();
}

void WinBase::paint()
{
    onPaint();
    PAINTSTRUCT ps;
    auto dc = BeginPaint(hwnd, &ps);
    BITMAPINFO bmpInfo = { sizeof(BITMAPINFOHEADER), w, -h, 1, 32, BI_RGB, h * 4 * w, 0, 0, 0, 0 };
    StretchDIBits(dc, 0, 0, w, h, 0, 0, w, h, &winPix.front(), &bmpInfo, DIB_RGB_COLORS, SRCCOPY);
    ReleaseDC(hwnd, dc);
    EndPaint(hwnd, &ps);
}

void WinBase::initTool() {
    toolMain = std::make_unique<ToolMain>();
    toolMain->Init();
    toolSub = std::make_unique<ToolSub>();
    toolSub->Init();
}

void WinBase::initWindow()
{
    static int num = 0;
    std::wstring clsName = std::format(L"ScreenCapture{}", num++);
    auto hinstance = App::Get()->instance;
    WNDCLASSEX wcx{};
    wcx.cbSize = sizeof(wcx);
    wcx.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wcx.lpfnWndProc = &WinBase::routeWinMsg;
    wcx.cbWndExtra = sizeof(WinBase*);
    wcx.hInstance = hinstance;
    wcx.hIcon = LoadIcon(hinstance, IDI_APPLICATION);
    wcx.hCursor = LoadCursor(hinstance, IDC_CROSS);
    wcx.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcx.lpszClassName = clsName.c_str();
    RegisterClassEx(&wcx);
    hwnd = CreateWindowEx(0, clsName.c_str(), clsName.c_str(),
        WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_POPUP, 
        x, y, w, h, NULL, NULL, hinstance, static_cast<LPVOID>(this));    
    ShowWindow(hwnd, SW_SHOW);
    App::Cursor(IDC_CROSS);
    UINT dpi = GetDpiForWindow(hwnd);
    scaleFactor = dpi / 96.0f;
}

void WinBase::initSurface()
{
    auto dataSize = h * w;
    winPix.resize(dataSize);
    canvasPix.resize(dataSize, 0x00000000);
    SkImageInfo info = SkImageInfo::MakeN32Premul(w, h);
    auto rowSize = w * sizeof(SkColor);
    winCanvas = SkCanvas::MakeRasterDirect(info, &winPix.front(), rowSize);
    canvas = SkCanvas::MakeRasterDirect(info, &canvasPix.front(), rowSize);
}

void WinBase::Refresh()
{
    if (refreshFlag.load()) {
        return;
    }
    else {
        refreshFlag.store(true);
        static auto duration{ std::chrono::milliseconds(15) };
        std::jthread t([this]() {
            std::this_thread::sleep_for(duration);
            InvalidateRect(hwnd, nullptr, false);
            refreshFlag.store(false);
            });    
        t.detach();
    }
}

void WinBase::UpdateState(const State& _state)
{
    statePre = state;
    state = _state;
}
void WinBase::Emit(const EventType& et, const uint32_t& msg) {
    PostMessage(hwnd, WM_APP + 1, (UINT)et, msg);
}
void WinBase::onLeftBtnDown(const int& x, const int& y)
{
    for (size_t i = 0; i < leftBtnDownHandlers.size(); i++)
    {
        leftBtnDownHandlers[i](x, y);
    }
}

void WinBase::onLeftBtnUp(const int& x, const int& y)
{
    for (size_t i = 0; i < leftBtnUpHandlers.size(); i++)
    {
        leftBtnUpHandlers[i](x, y);
    }
}

void WinBase::onMouseMove(const int& x, const int& y)
{
    for (size_t i = 0; i < mouseMoveHandlers.size(); i++)
    {
        mouseMoveHandlers[i](x, y);
    }
}

void WinBase::onMouseDrag(const int& x, const int& y)
{
    for (size_t i = 0; i < mouseDragHandlers.size(); i++)
    {
        mouseDragHandlers[i](x, y);
    }
}

void WinBase::onCustomMsg(const EventType& type, const uint32_t& msg)
{
    for (size_t i = 0; i < customEventHandlers.size(); i++)
    {
        customEventHandlers[i](type,msg);
    }
}

