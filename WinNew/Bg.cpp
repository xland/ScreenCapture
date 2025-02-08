#include <Windows.h>
#include <QWindow>
#include <QPainter>
#include "Bg.h"
#include "../App/NativeRect.h"
#include "../App/Util.h"

namespace Win
{
    Bg::Bg(Box* box) : QWidget(nullptr), box{box}
    {
        setWindowFlags(Qt::FramelessWindowHint);
        setAttribute(Qt::WA_DeleteOnClose);
    }

    Bg::~Bg()
    {

    }

    void Bg::fullWindow()
    {
        auto& rect = NativeRect::getDesktopRect();
		x = rect.x();
        y = rect.y();
        auto w = rect.width();
        auto h = rect.height();
		img = Util::printScreen(x, y, w, h);
        setFixedSize(w, h);
        show();
        //注意 必须先重置大小，再设置缩放比例
        auto hwnd = (HWND)winId();
        SetWindowPos(hwnd, nullptr, x, y, w, h, SWP_NOZORDER | SWP_SHOWWINDOW);
        //注意 这里必须用窗口的dpr来设置img的dpr，不能用主屏的dpr，此操作必须最后执行
        auto dpr = windowHandle()->devicePixelRatio();
        img.setDevicePixelRatio(dpr);
    }

    QImage Bg::grabImg(const int& x, const int& y, const int& w, const int& h)
    {
        return img.copy(x, y, w, h);
    }

    void Bg::paintEvent(QPaintEvent* event)
    {
        QPainter painter(this);
        painter.drawImage(QPoint(0, 0), img);
    }
}