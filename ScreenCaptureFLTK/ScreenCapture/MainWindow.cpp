#include "MainWindow.h"
#include <windowsx.h>
#include <windowsx.h>
#include <dwmapi.h>
#include <stdexcept>

namespace {

}

MainWindow::MainWindow(HINSTANCE hinstance,int x,int y,int w,int h) :Fl_Double_Window(x, y,w,h), 
    hinstance{ hinstance }, hwnd{nullptr},
    x{x},y{y},w{w},h{h}
{
    shotVirtualScreen();
    initWindowGeomtry();
}
MainWindow::~MainWindow()
{

}

void MainWindow::Init(HINSTANCE hinstance)
{
    int x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int h = GetSystemMetrics(SM_CYVIRTUALSCREEN);
	mainWindow = std::make_shared<MainWindow>(hinstance,x,y,w,h);
}

void MainWindow::initWindowGeomtry()
{
    show();
    HWND hwnd = fl_xid(this);
    auto borderlessStyle = WS_EX_TOOLWINDOW | WS_POPUP | WS_VISIBLE | WS_SYSMENU;
    SetWindowLongPtr(hwnd, GWL_STYLE, borderlessStyle);
    SetWindowPos(hwnd, HWND_TOP, x, y, w, h, SWP_FRAMECHANGED);    
}

void MainWindow::shotVirtualScreen()
{
    HDC     hScreen = GetDC(NULL);
    HDC     hDC = CreateCompatibleDC(hScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, w, h);
    DeleteObject(SelectObject(hDC, hBitmap));
    BOOL    bRet = BitBlt(hDC, 0, 0, w, h, hScreen, x, y, SRCCOPY);
    unsigned int dataSize = ((w * 32 + 31) / 32) * 4 * h;
    BITMAPINFO Info = { sizeof(BITMAPINFOHEADER), static_cast<long>(w), static_cast<long>(0 - h), 1, 32, BI_RGB, dataSize, 0, 0, 0, 0 };  
    pixels.resize(dataSize);
    int r = GetDIBits(hDC, hBitmap, 0, h, &pixels[0], &Info, DIB_RGB_COLORS);
    bgImg = new Fl_RGB_Image(pixels.data(), w, h,4);
    bgImg->alloc_array = 1;
    DeleteDC(hDC);
    ReleaseDC(NULL, hScreen);
    DeleteObject(hBitmap);

    //size_t arraySize = ((size_t)width * height) * 3;
    //uchar* p = new uchar[arraySize];
    //memset(p, 0, arraySize);
    //int line_size = ((3 * width + 3) / 4) * 4;      // each line is aligned on a DWORD (4 bytes)
    //uchar* dib = new uchar[line_size * height];  // create temporary buffer to read DIB
    //BITMAPINFO   bi;
    //bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    //bi.bmiHeader.biWidth = width;
    //bi.bmiHeader.biHeight = -height;           // negative => top-down DIB
    //bi.bmiHeader.biPlanes = 1;
    //bi.bmiHeader.biBitCount = 24;         // 24 bits RGB
    //bi.bmiHeader.biCompression = BI_RGB;
    //bi.bmiHeader.biSizeImage = 0;
    //bi.bmiHeader.biXPelsPerMeter = 0;
    //bi.bmiHeader.biYPelsPerMeter = 0;
    //bi.bmiHeader.biClrUsed = 0;
    //bi.bmiHeader.biClrImportant = 0;
    //GetDIBits(hDC, hBitmap, 0, height, dib, (BITMAPINFO*)&bi, DIB_RGB_COLORS);
    //for (int j = 0; j < height; j++) {
    //    const uchar* src = dib + j * line_size;                     // source line
    //    uchar* tg = p + j * 3 * width;       // target line
    //    for (int i = 0; i < width; i++) {
    //        uchar b = *src++;
    //        uchar g = *src++;
    //        *tg++ = *src++;   // R
    //        *tg++ = g;        // G
    //        *tg++ = b;        // B
    //    }
    //}
    //delete[] dib;
    //bgImg = new Fl_RGB_Image(p, width, height);
    //bgImg->alloc_array = 1;
    //bgImg->scale(width / 1.25, height / 1.25);
    
    
    // clean up
    //DeleteDC(hDC);
    //ReleaseDC(NULL, hScreen);
    //DeleteObject(hBitmap);
}

void MainWindow::draw()
{
//    //fl_draw_image(bgImg->, 0, 0, width, height);
//    //bgImg->scale()
//    //auto w = this->width();
//    //auto h = 
//
//
////    if (bgImg->w() > this->w() || bgImg->h() > this->h()) {
////        Fl_Image* temp;
////        if (bgImg->w() > bgImg->h()) {
////            temp = bgImg->copy(this->w(), this->h() * bgImg->h() / bgImg->w());
////        }
////        else {
////            temp = bgImg->copy(this->w() * bgImg->w() / bgImg->h(), this->h());
////        }
////        bgImg->release();
////        bgImg = (Fl_RGB_Image*)temp;
////    }
////    auto w = this->w();
////auto w1 = bgImg->w();
    //bgImg->draw(0, 0);
    fl_draw_image(pixels.data(), 0, 0, w, h, 4);
}