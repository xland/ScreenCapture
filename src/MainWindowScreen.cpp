#include "MainWindow.h"
#include <QScreen>
#include <QGuiApplication>
#include <Windows.h>
#include <qtransform.h>
#include <qapplication.h>
#include <qdesktopwidget.h>
#include <qwindow.h>
#include "qdebug.h"
#include <qtimer.h>


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
    this->setGeometry(this->screenRect);
    QTimer::singleShot(0, this, [this](){
        HWND hwnd = (HWND)this->winId();
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
    show();
}

void MainWindow::initWindowRects()
{
    EnumWindows([](HWND hwnd, LPARAM lparam)
        {
            if (!IsWindowVisible(hwnd)) return TRUE;
            if (IsIconic(hwnd)) return TRUE;
            if (GetWindowTextLength(hwnd) < 1) return TRUE;
            auto self = (MainWindow*)lparam;
            RECT rect;
            GetWindowRect(hwnd, &rect);
            QRect item;
            item.adjust(rect.left / self->maxRate, rect.top / self->maxRate, rect.right / self->maxRate, rect.bottom / self->maxRate);
            if (item.width() <= 6 || item.height() <= 6) {
                return TRUE;
            }
            self->windowRects.append(std::move(item));
            /*QPoint leftTop = self->transform.map(item.topLeft() / rate);
            item.moveTo(leftTop);
            item.setSize(item.size() / rate);
            instance->adjustRect2(item);*/
            //std::string title;
            //title.reserve(GetWindowTextLength(hwnd) + 1);
            //GetWindowTextA(hwnd, const_cast<CHAR*>(title.c_str()), title.capacity());            
            //OutputDebugStringA(title.c_str());
            //OutputDebugStringA("\r\n");            
            //QString a = QString::fromLatin1(title.data());
            //if (a.startsWith("asd.txt")) {
            //    QPainter p(self->desktopImage);
            //    p.setPen(QPen(QBrush(Qt::red), 3, Qt::SolidLine, Qt::RoundCap));
            //    p.setBrush(Qt::NoBrush);
            //    p.drawRect(item);
            //    //instance->desktopImage.save("desktopImage.png");
            //}

            return TRUE;
        }, (LPARAM)this);
}