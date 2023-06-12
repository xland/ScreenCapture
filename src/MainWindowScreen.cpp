#include "MainWindow.h"
#include <QScreen>
#include <QGuiApplication>
#include <Windows.h>
#include <dwmapi.h>
#include <qtransform.h>
#include <qapplication.h>
#include <qdesktopwidget.h>
#include <qwindow.h>
#include "qdebug.h"
#include <qtimer.h>
#include <QPainter>


void MainWindow::shotScreen()
{
    screenRect = QRect(GetSystemMetrics(SM_XVIRTUALSCREEN),
                       GetSystemMetrics(SM_YVIRTUALSCREEN),
                       GetSystemMetrics(SM_CXVIRTUALSCREEN),
                       GetSystemMetrics(SM_CYVIRTUALSCREEN));
    HDC hScreen = GetDC(NULL);
    HDC hDC = CreateCompatibleDC(hScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, screenRect.width(), screenRect.height());
    BOOL result = DeleteObject(SelectObject(hDC, hBitmap));
    if (!result) {
    }
    result = BitBlt(hDC, 0, 0, screenRect.width(), screenRect.height(), hScreen, screenRect.x(), screenRect.y(), SRCCOPY);
    if (!result) {

    }
    desktopImage = new QImage(screenRect.size(),QImage::Format_ARGB32_Premultiplied);
    unsigned int dataSize = ((screenRect.width() * 32 + 31) / 32) * 4 * screenRect.height();
    BITMAPINFO Info = { sizeof(BITMAPINFOHEADER), static_cast<long>(screenRect.width()), static_cast<long>(0-screenRect.height()), 1, 32, BI_RGB, dataSize, 0, 0, 0, 0 };
    int r = GetDIBits(hDC, hBitmap, 0, screenRect.height(), desktopImage->bits(), &Info, DIB_RGB_COLORS);
    if (r == 0) {
        
    }
    result = DeleteDC(hDC);
    if (!result) {
    }
    ReleaseDC(NULL, hScreen);
    result = DeleteObject(hBitmap);
    if (!result) {
    }
}

void MainWindow::adjustWindowToScreen()
{
    QTimer::singleShot(0, this, [this](){        
        SetWindowPos(hwnd,HWND_TOP,
                     this->screenRect.x(),
                     this->screenRect.y(),
                     this->screenRect.width(),
                     this->screenRect.height(),
                     SWP_HIDEWINDOW);
        SetWindowPos(hwnd,HWND_TOP,
                     this->screenRect.x(),
                     this->screenRect.y(),
                     this->screenRect.width(),
                     this->screenRect.height(),
                     SWP_SHOWWINDOW);                     
            int dpi = GetDpiForWindow(hwnd);
            switch (dpi) {
                case 96:
                    this->scaleFactor = 1.0;
                    break;
                case 120:
                    this->scaleFactor = 1.25;
                    break;
                case 144:
                    this->scaleFactor = 1.5;
                    break;
                case 168:
                    this->scaleFactor = 1.75;
                    break;
                case 192:
                    this->scaleFactor = 2.0;
                    break;
                default:
                    this->scaleFactor = 1.0;
                    break;
            }           
            this->desktopImage->setDevicePixelRatio(this->scaleFactor);
            emit shotScreenReady();            
    });    
    hwnd = (HWND)this->winId(); 
    bool flag = true;
    auto screens = QGuiApplication::screens();
    if(screens.count()>1){
        qreal rate0 = screens[0]->devicePixelRatio();
        qreal rate1 = screens[1]->devicePixelRatio();
        if(rate0 == 1.25 &&  rate1 >= 1.50 &&  rate1 <= 2.0){
            flag = false;
        }else if(rate0 == 1.5 && rate1 >= 1.75 &&  rate1 <= 2.0){
            flag = false;
        }else if(rate0 == 1.75 &&  rate1 >= 1.50 &&  rate1 <= 2.0){
            flag = false;
        }
    }
    if(flag){
        this->setFixedSize(this->screenRect.size());
    }
    //todo two both 1.75/1.5/1.25
    show();
}

void MainWindow::initWindowRects()
{
    EnumWindows([](HWND hwnd, LPARAM lparam)
        {
            if (!IsWindowVisible(hwnd)) return TRUE;
            if (IsIconic(hwnd)) return TRUE;
            if (GetWindowTextLength(hwnd) < 1) return TRUE;
            int flag = 1;
            DwmGetWindowAttribute(hwnd,DWMWA_CLOAKED,&flag,NULL);
            if(flag == 0){
                return TRUE;
            }
            auto self = (MainWindow*)lparam;
            RECT rect;
            GetWindowRect(hwnd, &rect);
            QRect item;
            item.adjust(rect.left,rect.top,rect.right,rect.bottom);            
            if (item.width() <= 6 || item.height() <= 6) {
                return TRUE;
            }
            self->windowRects.append(std::move(item));    
            return TRUE;      
        }, (LPARAM)this);
}

QPoint MainWindow::getNativeMousePos()
{
    POINT pos;
    GetCursorPos(&pos);
    ScreenToClient(hwnd, &pos);
    QPoint position(pos.x/scaleFactor,pos.y/scaleFactor);
    return position;
}