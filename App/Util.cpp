#include <Windows.h>
#include <windowsx.h>
#include "Util.h"
#include "../Win/WinBox.h"

QMouseEvent Util::createMouseEvent(const QEvent::Type& type, const Qt::MouseButton& btn)
{
    POINT pos; 
    GetCursorPos(&pos);
    auto isCtrl = GetKeyState(VK_CONTROL) & 0x8000;
    auto isShift = GetKeyState(VK_SHIFT) & 0x8000;
    Qt::KeyboardModifiers mf = Qt::NoModifier;
    if (isCtrl) mf = Qt::ControlModifier;
    if (isShift) mf = mf | Qt::ShiftModifier;
    QPointF p(pos.x, pos.y);
    QPointF g(pos.x, pos.y);
    return QMouseEvent(type, p, g, btn, btn, mf);
}

QMouseEvent Util::createMouseEvent(const LPARAM& lParam, const QEvent::Type& type, const Qt::MouseButton& btn)
{
    auto x = GET_X_LPARAM(lParam);
    auto y = GET_Y_LPARAM(lParam); 
    auto isCtrl = GetKeyState(VK_CONTROL) & 0x8000;
    auto isShift = GetKeyState(VK_SHIFT) & 0x8000;
    Qt::KeyboardModifiers mf = Qt::NoModifier;
    if (isCtrl) mf = Qt::ControlModifier;
    if (isShift) mf = mf | Qt::ShiftModifier;
    QPointF p(x, y);
    POINT pos;
    GetCursorPos(&pos);  //比ClientToScreen 效率要高
    QPointF g(pos.x, pos.y);
    return QMouseEvent(type, p, g, btn, btn, mf);
}

QImage Util::printWindow(WinBox* win)
{
    HDC hScreen = GetDC(NULL);
    HDC hDC = CreateCompatibleDC(hScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, win->w, win->h);
    DeleteObject(SelectObject(hDC, hBitmap));
    PrintWindow(win->hwnd, hDC, PW_RENDERFULLCONTENT);
    auto img = QImage(win->w, win->h, QImage::Format_ARGB32);
    BITMAPINFO info = { sizeof(BITMAPINFOHEADER), (long)win->w, 0 - (long)win->h, 1, 32, BI_RGB, (DWORD)win->w * 4 * win->h, 0, 0, 0, 0 };
    GetDIBits(hDC, hBitmap, 0, win->h, img.bits(), &info, DIB_RGB_COLORS);
    DeleteDC(hDC);
    DeleteObject(hBitmap);
    ReleaseDC(NULL, hScreen);
    return img;
}

QImage Util::printScreen(const int& x, const int& y, const int& w, const int& h)
{
    HDC hScreen = GetDC(NULL);
    HDC hDC = CreateCompatibleDC(hScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, w, h);
    DeleteObject(SelectObject(hDC, hBitmap));
    BOOL bRet = BitBlt(hDC, 0, 0, w, h, hScreen, x, y, SRCCOPY);
    auto img = QImage(w, h, QImage::Format_ARGB32);
    BITMAPINFO info = { sizeof(BITMAPINFOHEADER), (long)w, 0 - (long)h, 1, 32, BI_RGB, (DWORD)w * 4 * h, 0, 0, 0, 0 };
    GetDIBits(hDC, hBitmap, 0, h, img.bits(), &info, DIB_RGB_COLORS);
    DeleteDC(hDC);
    DeleteObject(hBitmap);
    ReleaseDC(NULL, hScreen);
    return img;
}


//QImage Util::printScreen(const int& x, const int& y, const int& w, const int& h)
//{
//    auto screens = QGuiApplication::screens();
//    for (int i = 0; i < screens.count(); i++) {
//        auto screen = screens[i];
//        QRect _screenRect = screen->geometry();
//        if (_screenRect.x() == 0 && _screenRect.y() == 0) {
//            auto dpr = screen->devicePixelRatio();
//            auto img = screen->grabWindow(0,
//                x / dpr,
//                y / dpr,
//                w / dpr,
//                h / dpr).toImage();
//            //img.save("allen123.png");
//            return img;
//        }
//    }
//}
