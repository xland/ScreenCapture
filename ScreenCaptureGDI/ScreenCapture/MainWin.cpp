#include "MainWin.h"
#include "spdlog/spdlog.h"

namespace {
    void log(const std::string&& message) {
#ifdef _DEBUG
        OutputDebugStringA(message.c_str());
        OutputDebugStringA("\r\n");
#else
        spdlog::get("logger")->info(message);
#endif
    }
}


MainWin::MainWin(HINSTANCE hinstance):hinstance{hinstance}
{
    initWindowGeomtry();
    shotScreen();
    createNativeWindow();
    ShowWindow(hwnd, SW_SHOW);
}
MainWin::~MainWin()
{ 
}


void MainWin::initWindowGeomtry()
{
    screenRect.x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    screenRect.y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    screenRect.w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    screenRect.h = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    maskRect[0].x = 0;
    maskRect[0].y = 0;
    maskRect[0].w = screenRect.w;
    maskRect[0].h = screenRect.h;
}
void MainWin::render()
{
}
void  MainWin::shotScreen()
{
    HDC hScreen = GetDC(NULL);
    HDC hDC = CreateCompatibleDC(hScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, screenRect.w, screenRect.h);
    BOOL result = DeleteObject(SelectObject(hDC, hBitmap));
    //if (!result) {
    //    log("DeleteObject error");
    //}
    //result = BitBlt(hDC, 0, 0, screenRect.w, screenRect.h, hScreen, screenRect.x, screenRect.y, SRCCOPY);
    //if (!result) {
    //    log("BitBlt error");
    //}
    //unsigned int dataSize = ((screenRect.w * 32 + 31) / 32) * 4 * screenRect.h;
    //BITMAPINFO Info = { sizeof(BITMAPINFOHEADER), static_cast<long>(screenRect.w), static_cast<long>(0- screenRect.h), 1, 32, BI_RGB, dataSize, 0, 0, 0, 0 };    
    //int r = GetDIBits(hDC, hBitmap, 0, screenRect.h, bgImg->pixels, &Info, DIB_RGB_COLORS);
    //if (r == 0) {
    //    log("GetDIBits error");
    //}
    result = DeleteDC(hDC);
    if (!result) {
        spdlog::get("logger")->info("DeleteDC error");
    }
    ReleaseDC(NULL, hScreen);
    result = DeleteObject(hBitmap);
    if (!result) {
        spdlog::get("logger")->info("DeleteObject(hBitmap)");
    }
}