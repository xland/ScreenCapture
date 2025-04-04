#include <QMouseEvent>
#include <QWindow>
#include <QPainter>
#include <QScreen>
#include <QPainterPath>
#include <QFontDatabase>
#include <Windows.h>

#include "Magnifier.h"
#include "../App/Util.h"
#include "Box.h"


Magnifier::Magnifier(Box* box) : QWidget(nullptr), box{ box }
{
    setAutoFillBackground(false);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setAttribute(Qt::WA_QuitOnClose, false);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAttribute(Qt::WA_TransparentForMouseEvents, true);
    setFixedSize(180, 170);
    setFocusPolicy(Qt::NoFocus);
    auto hwnd = (HWND)winId();
    LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
    SetWindowLong(hwnd, GWL_EXSTYLE, exStyle | WS_EX_TRANSPARENT);
}

Magnifier::~Magnifier()
{
}

void Magnifier::mouseMove(QMouseEvent* event)
{
    QPoint pos = event->pos();
    int span{ 10 };
    if (Util::posInScreen(pos.x() + width() + span, pos.y() + height() + span)) {
        move(pos.x() + span, pos.y() + span);
    }
    else if (Util::posInScreen(pos.x() - width() - span, pos.y() + height() + span)) {
        move(pos.x() - span - width(), pos.y() + span);
    }
    else if (Util::posInScreen(pos.x() + width() + span, pos.y() - height() - span)) {
        move(pos.x() + span, pos.y() - height() - span);
    }
    else if (Util::posInScreen(pos.x() - width() - span, pos.y() - height() - span)) {
        move(pos.x() - span - width(), pos.y() - height() - span);
    }
    update();
    if (!isVisible()) {
        show();
    }
}

void Magnifier::paintEvent(QPaintEvent* event)
{
    POINT pos;
    GetCursorPos(&pos);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    //背景
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(0, 0, 0, 168));
    painter.drawRect(rect());
    //放大后的目标矩形区域的图像
    auto img = box->bg->grabImg(pos.x - box->bg->x - 20, pos.y - box->bg->y - 10, 36, 20);
    painter.drawImage(QRect(0, 0, 180, 100), img);//放大了5倍
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
    font.setPointSizeF(7.0);
    painter.setFont(font);
    auto tarColor = img.pixelColor(img.rect().center());
    painter.drawText(QPoint(8, 114), QString("HEX (Ctrl+H) : %1").arg(tarColor.name().toUpper()));
    painter.drawText(QPoint(8, 130), QString("RGB (Ctrl+R) : %1,%2,%3").arg(tarColor.red()).arg(tarColor.green()).arg(tarColor.blue()));
    QColor cmyk = tarColor.toCmyk();
    painter.drawText(QPoint(8, 146), QString("CMYK (Ctrl+K) : %1,%2,%3,%4").arg(cmyk.cyan()).arg(cmyk.magenta()).arg(cmyk.yellow()).arg(cmyk.black()));
    painter.drawText(QPoint(8, 162), QString("POS (Ctrl+P) : X:%1 Y:%2").arg(pos.x - box->bg->padding).arg(pos.y - box->bg->padding));
    //边框
    painter.setPen(QPen(QColor(0, 0, 0, 168), 1));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(rect());
}

void Magnifier::closeEvent(QCloseEvent* event)
{
    deleteLater();
    box->magnifier = nullptr;
}