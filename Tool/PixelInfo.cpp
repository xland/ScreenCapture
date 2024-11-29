#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
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
    setFixedSize(200, 170);
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
        auto win = WinFull::get();
        auto x = pos.x() + 10;
        auto y = pos.y() + 10;
        if (x + width() > win->width()) {
            x = pos.x() - 20 - width();
        }
        if (y + height() > win->height()) {
            y = pos.y() - 20 - height();
        }
        move(x,y);
        update();
        
    }
}

void PixelInfo::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    //painter.fillRect(rect(),QColor(66,66,66,66));
    auto win = WinFull::get();
    auto dpr = win->img.devicePixelRatio();
    QPoint pos = QCursor::pos();
    QRect tarRect((pos.x() - 20) * dpr, (pos.y() - 10) * dpr, 40 * dpr, 20 * dpr);
    {
        auto img = win->img.copy(tarRect);
        img = img.scaled(200 * dpr, 100 * dpr);
        painter.drawImage(0, 0, img);
    }
    painter.setPen(QPen(QColor(0,0,0,168), 1));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(QRect(0, 0,200,100));

    QPainterPath path;
    path.addRect(QRect(0, 46, 200, 8));
    path.addRect(QRect(96, 0, 8, 100));
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(22, 119, 255, 110));
    painter.drawPath(path);


    painter.setBrush(QColor(0, 0, 0, 168));
    painter.drawRect(QRect(0, 100, 200, 80));

    painter.setBrush(Qt::NoBrush);
    painter.setPen(Qt::white);
    QFont font = painter.font();
    font.setPointSize(8);
    painter.setFont(font);
    auto tarColor = win->img.pixelColor(pos.x() * dpr, pos.y() * dpr);
    painter.drawText(QPoint(8, 118), QString("POS (Ctrl+P) : X:%1 Y:%2").arg(std::round(pos.x()*dpr)).arg(std::round(pos.y()*dpr)));
    painter.drawText(QPoint(8, 138), QString("RGB (Ctrl+R) : %1,%2,%3").arg(tarColor.red()).arg(tarColor.green()).arg(tarColor.blue()));
    painter.drawText(QPoint(8, 158), QString("HEX (Ctrl+H) : %1").arg(tarColor.name().toUpper()));
}
