#include <QMouseEvent>
#include <QWindow>
#include <QPainter>
#include <QScreen>
#include <QPainterPath>
#include <QFontDatabase>
#include <Windows.h>

#include "PixelInfo.h"
#include "../Win/WinFull.h"

PixelInfo::PixelInfo(WinFull* win, QWidget* parent) : QWidget(parent),win{win}
{
    setAutoFillBackground(false);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setAttribute(Qt::WA_QuitOnClose, false);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAttribute(Qt::WA_TransparentForMouseEvents, true);
    setFixedSize(180, 170);
    auto hwnd = (HWND)winId();
    LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
    SetWindowLong(hwnd, GWL_EXSTYLE, exStyle | WS_EX_TRANSPARENT);
}

PixelInfo::~PixelInfo()
{
}

bool PixelInfo::posInScreen(const int& x, const int& y)
{
    QList<QScreen*> screens = QGuiApplication::screens();
    for (QScreen* screen : screens) {
        if (screen->geometry().contains(x, y)) {
            return true;
        }
    }
    return false;
}

void PixelInfo::mouseMove(QMouseEvent* event)
{
    if (win->state == State::start) {
        QPoint pos = QCursor::pos();
        int span{ 10 };
        if (posInScreen(pos.x() + width() + span, pos.y() + height() + span)) {
            move(pos.x() + span, pos.y() + span);
        }
        else if (posInScreen(pos.x() - width() - span, pos.y() + height() + span)) {
            move(pos.x() - span - width(), pos.y() + span);
        }
        else if (posInScreen(pos.x() + width() + span, pos.y() - height() - span)) {
            move(pos.x() + span, pos.y() - height() - span);
        }
        else if (posInScreen(pos.x() - width() - span, pos.y() - height() - span)) {
            move(pos.x() - span - width(), pos.y() - height() - span);
        }
        nativePos = event->pos();
        update();
        if (!isVisible()) {
            show();
        }
    }
}

void PixelInfo::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    //背景
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(0, 0, 0, 168));
    painter.drawRect(rect());
    //放大后的目标矩形区域的图像
    QRect tarRect((nativePos.x() - 20), (nativePos.y() - 10), 36, 20);
    auto img = win->img.copy(tarRect);
    painter.drawImage(QRect(0,0,180,100),img);
    //十字架准星
    QPainterPath path;
    path.addRect(QRect(0, 46, 180, 8));
    path.addRect(QRect(86, 0, 8, 100));
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(22, 119, 255, 110));
    painter.drawPath(path);
    //文字信息
    painter.setBrush(Qt::NoBrush);
    painter.setPen(Qt::white);
    auto font = painter.font();
    font.setPointSizeF(8.0);
    painter.setFont(font);
    auto tarColor = img.pixelColor(img.rect().center());
    painter.drawText(QPoint(8, 118), QString("HEX (Ctrl+H) : %1").arg(tarColor.name().toUpper()));
    painter.drawText(QPoint(8, 138), QString("RGB (Ctrl+R) : %1,%2,%3").arg(tarColor.red()).arg(tarColor.green()).arg(tarColor.blue()));
    painter.drawText(QPoint(8, 158), QString("POS (Ctrl+P) : X:%1 Y:%2").arg(nativePos.x()).arg(nativePos.y()));
    //边框
    painter.setPen(QPen(QColor(0, 0, 0, 168), 1));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(rect());
}

void PixelInfo::closeEvent(QCloseEvent* event)
{
    win->pixelInfo = nullptr;
    win->releaseImg();
    deleteLater();
}


