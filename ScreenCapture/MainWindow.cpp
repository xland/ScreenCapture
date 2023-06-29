#include "MainWindow.h"
#include <windowsx.h>
#include <dwmapi.h>
#include <stdexcept>
#include <fstream>
#include <FL/FL_Box.H>

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
    pixels = new std::uint8_t[dataSize];
    int r = GetDIBits(hDC, hBitmap, 0, h, &pixels[0], &Info, DIB_RGB_COLORS);
    for (int x = 0; x < dataSize; x += 4)
    {
        auto r = pixels[x + 2];
        auto b = pixels[x];
        pixels[x] = r;
        pixels[x + 2] = b;
    }
    DeleteDC(hDC);
    ReleaseDC(NULL, hScreen);
    DeleteObject(hBitmap);  

    Fl_Box* mypicturebox = new Fl_Box(0, 0, w, h);
    Fl_RGB_Image myImage(pixels, w, h);
    mypicturebox->image(myImage);
    mypicturebox->redraw();
    this->add(mypicturebox);
}

//void MainWindow::draw()
//{
//    fl_draw_pixmap(pixels, 0, 0);
//}