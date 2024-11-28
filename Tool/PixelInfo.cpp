#include <QMouseEvent>
#include <QPainter>
#include <Windows.h>

#include "PixelInfo.h"
#include "../Win/WinFull.h"

PixelInfo::PixelInfo(QWidget* parent) : QWidget(parent)
{
    setAutoFillBackground(false);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setAttribute(Qt::WA_QuitOnClose, false);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAttribute(Qt::WA_TransparentForMouseEvents, true);
    setFixedSize(300, 600);
    QPoint pos = QCursor::pos();
    move(pos.x() + 10, pos.y() + 10);
    show();
    auto hwnd = (HWND)winId();
    LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
    SetWindowLong(hwnd, GWL_EXSTYLE, exStyle | WS_EX_TRANSPARENT);
}

PixelInfo::~PixelInfo()
{
}

void PixelInfo::mouseMove(QMouseEvent* event)
{
    if (isVisible()) {
        auto pos = event->pos();
        move(pos.x() + 10, pos.y() + 10);

        auto win = WinFull::get();
        QRect tarRect(pos.x() - 150, pos.y() - 150, 300, 300);
        img = win->img.copy(tarRect);

        update();
        
    }
}

void PixelInfo::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.fillRect(rect(),QColor(66,66,66,66));
    //auto win = WinFull::get();
    //QPoint pos = QCursor::pos() / win->img.devicePixelRatio();
    //QRect tarRect(pos.x() - 150, pos.y() - 150, 300, 300);
    //QImage img = win->img.copy(tarRect);
    painter.drawImage(0, 0, img);
}
