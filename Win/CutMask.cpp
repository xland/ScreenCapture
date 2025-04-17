#include <QWindow>
#include <dwmapi.h>
#include <shellscalingapi.h>
#include <QApplication>
#include <QTimer>
#include <QPainterPath>

#include "CutMask.h"
#include "WinFull.h"
#include "../App/Util.h"
#include "../App/NativeRect.h"
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
            auto lt = Util::getQtPoint(rect.left, rect.top);
            auto rb = Util::getQtPoint(rect.right, rect.bottom);
            lt = win->mapFromGlobal(lt);
            rb = win->mapFromGlobal(rb);
            self->rectWins.push_back(QRect(lt, rb));
            return TRUE;
        }, (LPARAM)this);
}

void CutMask::mousePress(QMouseEvent* event)
{
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

void CutMask::moveMaskRect(const QPoint& pos)
{
    auto span = pos - posPress;
    posPress = pos;
    auto target = rectMask.topLeft() + span;
    auto win = (WinFull*)parent();
    if (target.x() < 0) {
        target.setX(0);
        rectMask.moveTo(target);
        return;
    }
    if (target.y() < 0) {
        target.setY(0);
        rectMask.moveTo(target);
        return;
    }
    if (target.x() + rectMask.width() > win->width()) {
        target.setX(win->width() - rectMask.width());
        rectMask.moveTo(target);
        return;
    }
    if (target.y() + rectMask.height() > win->height()) {
        target.setY(win->height() - rectMask.height());
        rectMask.moveTo(target);
        return;
    }
    rectMask.moveTo(target);
}


void CutMask::mouseRelease(QMouseEvent* event)
{
    auto win = (WinFull*)parent();
    if (win->state == State::mask || (win->state >= State::tool && !win->toolMain->isVisible()) )
    {
        win->state = State::tool;
        //win->showToolMain();
        update();
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
}
void CutMask::paint(QPainter& p)
{
    auto win = (WinFull*)parent();
    p.setBrush(QColor(0, 0, 0, 120));
    QColor borderColor(22, 118, 255);
    p.setPen(QPen(QBrush(borderColor), 1.5));
    QPainterPath path;
    path.addRect(-1, -1, win->w + 1, win->h + 1);
    path.addRect(rectMask);
    p.drawPath(path);
    if (win->state != State::mask) return;
    if (QApplication::mouseButtons() & Qt::LeftButton) return;

    //绘制边框上的拖动圆点
    p.setBrush(borderColor);
    p.setPen(QPen(QBrush(QColor(255, 255, 255)), 1));
    auto r{ 3 };
    auto hw{ rectMask.width() / 2 };
    auto hh{ rectMask.height() / 2 };
    p.drawEllipse(rectMask.topLeft().toPointF(), r, r);
    p.drawEllipse(QPointF(rectMask.left() + hw, rectMask.top()), r, r);
    p.drawEllipse(rectMask.topRight().toPointF(), r, r);
    p.drawEllipse(QPointF(rectMask.right(), rectMask.top() + hh), r, r);
    p.drawEllipse(rectMask.bottomRight().toPointF(), r, r);
    p.drawEllipse(QPointF(rectMask.left() + hw, rectMask.bottom()), r, r);
    p.drawEllipse(rectMask.bottomLeft().toPointF(), r, r);
    p.drawEllipse(QPointF(rectMask.left(), rectMask.top() + hh), r, r);

    //绘制截图区域位置和大小
    auto text = QString("X:%1 Y:%2 R:%3 B:%4 W:%5 H:%6")
        .arg(rectMask.x()).arg(rectMask.y())
        .arg(rectMask.right()).arg(rectMask.bottom())
        .arg(rectMask.width()).arg(rectMask.height());
    auto font = Util::getTextFont(10);
    QFontMetrics fm(*font);
    p.setFont(*font);
    int w = fm.horizontalAdvance(text);
    if (win->y < 0) win->y = rectMask.y() + 4;
    QRect rect(rectMask.x(), rectMask.y() - 25, w + 14, 22);
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
void CutMask::changeMousePosState(const int& x, const int& y)
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
void CutMask::changeMousePosState2(const int& x, const int& y)
{
    auto x1{ maskRect.x() - maskStroke }, x2{ x1 + maskStroke * 3 };
    auto x3{ maskRect.right() - maskStroke }, x4{ x3 + maskStroke * 3 };
    auto y1{ maskRect.y() - maskStroke }, y2{ y1 + maskStroke * 3 };
    auto y3{ maskRect.bottom() - maskStroke }, y4{ y3 + maskStroke * 3 };
    if (x >= x1 && x <= x2 && y >= y1 && y <= y2) {
        QGuiApplication::setOverrideCursor(Qt::SizeFDiagCursor);
        mousePosState = 1;
    }
    else if (x >= x2 && x <= x3 && y >= y1 && y <= y2) {
        QGuiApplication::setOverrideCursor(Qt::SizeVerCursor);
        mousePosState = 2;
    }
    else if (x >= x3 && x <= x4 && y >= y1 && y <= y2)
    {
        QGuiApplication::setOverrideCursor(Qt::SizeBDiagCursor);
        mousePosState = 3;
    }
    else if (x >= x3 && x <= x4 && y >= y2 && y <= y3)
    {
        QGuiApplication::setOverrideCursor(Qt::SizeHorCursor);
        mousePosState = 4;
    }
    else if (x >= x3 && x <= x4 && y >= y3 && y <= y4)
    {
        QGuiApplication::setOverrideCursor(Qt::SizeFDiagCursor);
        mousePosState = 5;
    }
    else if (x >= x2 && x <= x3 && y >= y3 && y <= y4)
    {
        QGuiApplication::setOverrideCursor(Qt::SizeVerCursor);
        mousePosState = 6;
    }
    else if (x >= x1 && x <= x2 && y >= y3 && y <= y4)
    {
        QGuiApplication::setOverrideCursor(Qt::SizeBDiagCursor);
        mousePosState = 7;
    }
    else if (x >= x1 && x <= x2 && y >= y2 && y <= y3)
    {
        QGuiApplication::setOverrideCursor(Qt::SizeHorCursor);
        mousePosState = 8;
    }
    else {
        QGuiApplication::restoreOverrideCursor();
        mousePosState = -1;
    }
}
void CutMask::paintMaskRectInfo(QPainter& p)
{
    //绘制截图区域位置和大小
    auto text = QString("X:%1 Y:%2 R:%3 B:%4 W:%5 H:%6")
        .arg(maskRect.x()).arg(maskRect.y())
        .arg(maskRect.right()).arg(maskRect.bottom())
        .arg(maskRect.width()).arg(maskRect.height());
    auto font = p.font();
    font.setPointSizeF(12.f);
    p.setFont(font);
    QFontMetrics fm(font);
    QRect textRect = fm.boundingRect(text);
    int w = textRect.width();
    int h = textRect.height();

	auto y = maskRect.y() - h - 12;
    auto x = maskRect.x() + 4;
	if (y < 0) y = maskRect.y() + 4;
    QRect rect(x, y, w + 16, h + 8);


    p.setBrush(QColor(0, 0, 0, 120));
    p.setPen(Qt::NoPen);
    p.drawRect(rect);
    p.setPen(QPen(QBrush(Qt::white), 1));
    p.setBrush(Qt::NoBrush);
    p.drawText(rect, Qt::AlignCenter, text);
}

void CutMask::paintMaskRectBorder(QPainter& p)
{
    //绘制透明区域的边框
    p.setCompositionMode(QPainter::CompositionMode_SourceOver);
    QColor borderColor(22, 118, 255);
    p.setPen(QPen(QBrush(borderColor), maskStroke));
    p.setBrush(Qt::NoBrush);
    p.drawRect(maskRect);
    auto win = (WinFull*)parent();
    if (win->state == State::tool) {
        //绘制边框上的拖动圆点
        p.setBrush(borderColor);
        p.setPen(Qt::NoPen);
        auto r{ 2 * maskStroke };
        auto hw{ maskRect.width() / 2 };
        auto hh{ maskRect.height() / 2 };
        p.drawEllipse(maskRect.topLeft().toPointF(), r, r);
        p.drawEllipse(QPointF(maskRect.left() + hw, maskRect.top()), r, r);
        p.drawEllipse(maskRect.topRight().toPointF(), r, r);
        p.drawEllipse(QPointF(maskRect.right(), maskRect.top() + hh), r, r);
        p.drawEllipse(maskRect.bottomRight().toPointF(), r, r);
        p.drawEllipse(QPointF(maskRect.left() + hw, maskRect.bottom()), r, r);
        p.drawEllipse(maskRect.bottomLeft().toPointF(), r, r);
        p.drawEllipse(QPointF(maskRect.left(), maskRect.top() + hh), r, r);
    }
}

void CutMask::moveMaskRect(const QPoint& pos)
{
	auto span = pos - posPress;
	auto target = maskRect.topLeft()+span;
	posPress = pos;
	if (target.x() < 0 || target.y()<0) return;
	if (target.x() + maskRect.width() > w || target.y() + maskRect.height() > h) return;
    maskRect.moveTo(maskRect.topLeft() + span);
}


