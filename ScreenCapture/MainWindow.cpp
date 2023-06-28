#include "MainWindow.h"
#include <windowsx.h>
#include <windowsx.h>
#include <dwmapi.h>
#include <stdexcept>
#include <fstream>

namespace {

}

MainWindow::MainWindow(HINSTANCE hinstance,int x,int y,int w,int h) :Fl_Double_Window(x, y,w,h), 
    hinstance{ hinstance },
    x{x},y{y},w{w},h{h}
{
    shotVirtualScreen();
    initWindowGeomtry();
}
MainWindow::~MainWindow()
{

}

void MainWindow::initWindowGeomtry()
{
    show();
    hwnd = fl_xid(this);
    auto borderlessStyle = WS_EX_TOOLWINDOW | WS_POPUP |WS_VISIBLE | WS_SYSMENU;
    SetWindowLongPtr(hwnd, GWL_STYLE, borderlessStyle);
    SetWindowPos(hwnd, HWND_TOP, x, y, w, h, SWP_FRAMECHANGED);
}

void MainWindow::shotVirtualScreen()
{
    HDC hScreen = GetDC(NULL);
    HDC hDC = CreateCompatibleDC(hScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, w, h);
    DeleteObject(SelectObject(hDC, hBitmap));
    BOOL bRet = BitBlt(hDC, 0, 0, w, h, hScreen, x, y, SRCCOPY);
    unsigned int dataSize = ((w * 32 + 31) / 32) * 4 * h;
    BITMAPINFO Info = { sizeof(BITMAPINFOHEADER), static_cast<long>(w), static_cast<long>(0 - h), 1, 32, BI_RGB, dataSize, 0, 0, 0, 0 };     
    
    pixels.resize(dataSize);
    int r = GetDIBits(hDC, hBitmap, 0, h, &pixels[0], &Info, DIB_RGB_COLORS);
    bgImg = new Fl_Bitmap(pixels.data(),dataSize, w, h);
    //bgImg = new Fl_RGB_Image(pixels.data(), w, h,4);
    bgImg->alloc_array = 1;

    DeleteDC(hDC);
    ReleaseDC(NULL, hScreen);
    DeleteObject(hBitmap);  
}

void MainWindow::draw()
{
    fl_draw_image(pixels.data(), 0, 0, w, h, 4);
}