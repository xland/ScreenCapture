#include <QWindow>
#include <dwmapi.h>
#include <shellscalingapi.h>
#include <QApplication>
#include <QTimer>
#include <QPainterPath>

#include "CutMask.h"
#include "WinFull.h"
#include "../App/Util.h"
#include "../Tool/ToolMain.h"
#include "../Tool/ToolSub.h"
#include "PixelInfo.h"

CutMask::CutMask(QObject* parent) : QObject(parent)
{
    initWinRect();
}
CutMask::~CutMask()
{
}
void CutMask::initWinRect()
{
    rectWins.clear();
    EnumWindows([](HWND hwnd, LPARAM lparam)
        {
            if (!hwnd) return TRUE;
            if (!IsWindowVisible(hwnd)) return TRUE;
            if (IsIconic(hwnd)) return TRUE;
            if (GetWindowTextLength(hwnd) < 1) return TRUE;
            RECT rect;
            DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &rect, sizeof(RECT));
            if (rect.right - rect.left <= 6 || rect.bottom - rect.top <= 6) {
                return TRUE;
            }
            auto self = (CutMask*)lparam;
            auto win = (WinFull*)self->parent();
            if (rect.left < win->x) rect.left = win->x;
            if (rect.top < win->y) rect.top = win->y;
            if (rect.right > win->x + win->w) rect.right = win->x + win->w;
            if (rect.bottom > win->y + win->h) rect.bottom = win->y + win->h;

            QPoint lt(rect.left - win->x, rect.top - win->y);
            QPoint rb(rect.right - win->x, rect.bottom - win->y);
            auto dpr = win->devicePixelRatio();
            self->rectWins.push_back(QRect(lt/dpr, rb/dpr));
            return TRUE;
        }, (LPARAM)this);
}

void CutMask::mousePress(QMouseEvent* event)
{
    auto win = (WinFull*)parent();
    posPress = event->pos();
    changeRectMask(posPress);
}

void CutMask::mouseDrag(QMouseEvent* event)
{
    auto pos = event->pos();
    auto win = (WinFull*)parent();
    if (win->state == State::start) {
        if (pos == posPress) return;
        rectMask.setCoords(posPress.x(), posPress.y(), pos.x(), pos.y());
        win->update();
    }
    else if (win->state == State::tool) {
        if (mouseState == 0)
        {
            moveMaskRect(pos);
        }
        else {
            changeRectMask(pos);
        }
        win->update();
    }
}
void CutMask::mouseMove(QMouseEvent* event)
{
    auto pos = event->pos();
    auto win = (WinFull*)parent();
    if (win->state == State::start) {
        win->pixelInfo->mouseMove(pos);
        for (const auto& rect : rectWins)
        {
            if (rect.contains(pos)) {
                if (rectMask == rect) return;
                rectMask = rect;
                win->update();
                return;
            }
        }
    }
    else if (win->state <= State::tool) {
        changeMouseState(pos.x(), pos.y());
    }
    else {
       
    }
}
void CutMask::mouseRelease(QMouseEvent* event)
{
    auto win = (WinFull*)parent();
    if (win->state == State::mask)
    {
        rectMask = rectMask.normalized();
        win->update();
    }
}
void CutMask::moveMaskRect(const QPoint& pos)
{
    auto span = pos - posPress;
    posPress = pos;
    auto target = rectMask.topLeft() + span;
    auto win = (WinFull*)parent();
    auto flag1 = target.x() < 0;
    auto flag2 = target.y() < 0;
    if (flag1 || flag2) {
        if (flag1)target.setX(0);
        if (flag2)target.setY(0);
        rectMask.moveTo(target);
        return;
    }
    flag1 = target.x() + rectMask.width() > win->width();
    flag2 = target.y() + rectMask.height() > win->height();
    if (flag1 || flag2) {
        if(flag1)target.setX(win->width() - rectMask.width());
        if(flag2)target.setY(win->height() - rectMask.height());
        rectMask.moveTo(target);
        return;
    }
    rectMask.moveTo(target);
}


void CutMask::paint(QPainter& p)
{
    auto win = (WinFull*)parent();
    p.setBrush(QColor(0, 0, 0, 120));
    QColor borderColor(22, 118, 255);
    p.setPen(QPen(QBrush(borderColor), maskStroke));
    QPainterPath path;
    path.addRect(-1, -1, win->w + 1, win->h + 1);
    path.addRect(rectMask);
    p.drawPath(path);
    //绘制截图区域位置和大小
    auto text = QString("X:%1 Y:%2 R:%3 B:%4 W:%5 H:%6")
        .arg(rectMask.x()).arg(rectMask.y())
        .arg(rectMask.right()).arg(rectMask.bottom())
        .arg(rectMask.width()).arg(rectMask.height());
    auto font = Util::getTextFont(10);
    QFontMetrics fm(*font);
    p.setFont(*font);
    int w = fm.horizontalAdvance(text);
    QRect rect;
    if (rectMask.y() - 25 >= 0) {
        rect = QRect(rectMask.x(), rectMask.y() - 25, w + 14, 22);
    }
    else {
        rect = QRect(rectMask.x()+2, rectMask.y() + 2, w + 14, 22);
    }
    p.setBrush(QColor(0, 0, 0, 120));
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(rect, 3, 3);
    p.setPen(QPen(QBrush(Qt::white), 1));
    p.setBrush(Qt::NoBrush);
    p.drawText(rect, Qt::AlignCenter, text);
}
void CutMask::changeRectMask(const QPoint& pos)
{
    if (mouseState == 1)
    {
        rectMask.setTopLeft(pos);
    }
    else if (mouseState == 2)
    {
        rectMask.setTop(pos.y());
    }
    else if (mouseState == 3)
    {
        rectMask.setTopRight(pos);
    }
    else if (mouseState == 4)
    {
        rectMask.setRight(pos.x());
    }
    else if (mouseState == 5)
    {
        rectMask.setBottomRight(pos);
    }
    else if (mouseState == 6)
    {
        rectMask.setBottom(pos.y());
    }
    else if (mouseState == 7)
    {
        rectMask.setBottomLeft(pos);
    }
    else if (mouseState == 8)
    {
        rectMask.setLeft(pos.x());
    }
    auto win = (WinFull*)parent();
    win->update();
}
void CutMask::changeMouseState(const int& x, const int& y)
{
    auto leftX = rectMask.left(); auto topY = rectMask.top();
    auto rightX = rectMask.right(); auto bottomY = rectMask.bottom();
    auto win = (WinFull*)parent();
    if (x > leftX + 1 && y > topY + 1 && x < rightX - 1 && y < bottomY - 1) //不然截图区域上顶天，下顶地的时候没法改变高度
    {
        win->setCursor(Qt::SizeAllCursor);
        mouseState = 0;
    }
    else if (x <= leftX && y <= topY)
    {
        win->setCursor(Qt::SizeFDiagCursor);
        mouseState = 1;
    }
    else if (x >= leftX && x <= rightX && y <= topY)
    {
        win->setCursor(Qt::SizeVerCursor);
        mouseState = 2;
    }
    else if (x >= rightX && y <= topY)
    {
        win->setCursor(Qt::SizeBDiagCursor);
        mouseState = 3;
    }
    else if (x >= rightX && y >= topY && y <= bottomY)
    {
        win->setCursor(Qt::SizeHorCursor);
        mouseState = 4;
    }
    else if (x >= rightX && y >= bottomY)
    {
        win->setCursor(Qt::SizeFDiagCursor);
        mouseState = 5;
    }
    else if (x >= leftX && x <= rightX && y >= bottomY)
    {
        win->setCursor(Qt::SizeVerCursor);
        mouseState = 6;
    }
    else if (x <= leftX && y >= bottomY)
    {
        win->setCursor(Qt::SizeBDiagCursor);
        mouseState = 7;
    }
    else if (x <= leftX && y <= bottomY && y >= topY)
    {
        win->setCursor(Qt::SizeHorCursor);
        mouseState = 8;
    }
}
