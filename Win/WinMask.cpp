#include <QWindow>
#include <dwmapi.h>
#include <shellscalingapi.h>

#include "WinMask.h"
#include "WinFull.h"
#include "../Tool/ToolMain.h"
#include "../Tool/ToolSub.h"
#include "../Tool/PixelInfo.h"

WinMask::WinMask(QObject* parent) : WinBase(parent)
{
    auto winFull = (WinFull*)parent;
    initSizeByWin(winFull);
    initWinRects();
    maskStroke = maskStroke * winFull->dpr;
    initWindow();
    show();
}

WinMask::~WinMask()
{
}

void WinMask::mousePress(QMouseEvent* event)
{
    auto win = (WinFull*)parent();
    if (win->state == State::start)
    {
        posPress = event->pos();
        win->state = State::mask;
        win->pixelInfo->close();
        win->pixelInfo = nullptr;
        event->accept();
        return;
    }
    if (win->state == State::tool)
    {
        posPress = event->pos();
        win->toolMain->hide();
        if (mousePosState != 0)
        {
            changeMaskRect(posPress);
        }
        event->accept();
        return;
    }
    if (win->state > State::tool && mousePosState > 0) {
        posPress = event->pos();
        win->toolMain->hide();
        win->toolSub->hide();
        event->accept();
        return;
    }
}

void WinMask::mouseDrag(QMouseEvent* event)
{
    auto father = (WinFull*)parent();
    if (father->state == State::mask)
    {
        auto pos = event->pos();
        maskRect.setCoords(posPress.x(), posPress.y(), pos.x(),pos.y());
        maskRect = maskRect.normalized();
        update();
        return;
    }
    if (father->state == State::tool)
    {
        if (mousePosState == 0)
        {
            auto pos = event->pos();
            auto span = pos - posPress;
            maskRect.moveTo(maskRect.topLeft() + span);
            posPress = pos;
            update();
        }
        else
        {
            auto pos = event->pos();
            changeMaskRect(pos);
        }
        return;
    }
    if (father->state > State::tool && mousePosState > 0) {
        auto pos = event->pos();
        changeMaskRect(pos);
        return;
    }
}

void WinMask::mouseRelease(QMouseEvent* event)
{
    auto win = (WinFull*)parent();
    if (win->state == State::mask)
    {
        win->state = State::tool;
        win->showToolMain();
        update(true);
    }
    else if (win->state == State::tool)
    {
        win->showToolMain();
    }
    if (win->state > State::tool && mousePosState > 0) {
        win->showToolMain();
        win->showToolSub();
        return;
    }
}

void WinMask::mouseMove(QMouseEvent* event)
{
    auto father = (WinFull*)parent();
    auto pos = event->pos();
    if (father->state == State::start)
    {
        event->accept();    
        for (int i = 0; i < winNativeRects.size(); i++)
        {
            if (winNativeRects[i].contains(event->pos())) {
                if (mouseInRectIndex == i) return;
                mouseInRectIndex = i;
                maskRect = winNativeRects[i];
                update();
                return;
            }
        }
    }
    else if (father->state == State::tool)
    {
        auto pos = event->pos();
        changeMousePosState(pos.x(), pos.y());
        event->accept();
    }
    else if (father->state > State::tool) {
        auto pos = event->pos();
        changeMousePosState2(pos.x(), pos.y());
        if (mousePosState != -1) {
            event->accept();
            return;
        }
    }
}
void WinMask::update(bool isMouseup)
{
    if (img.isNull()) {
        img = QImage(w, h, QImage::Format_ARGB32_Premultiplied);
    }
    //绘制半透明和透明区域
    img.fill(QColor(0, 0, 0, 120));
    QPainter p(&img);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setCompositionMode(QPainter::CompositionMode_Clear);
    p.setBrush(Qt::transparent);
    p.drawRect(maskRect);
    //绘制透明区域的边框
    p.setCompositionMode(QPainter::CompositionMode_SourceOver);
    QColor borderColor(22, 119, 255);
    p.setPen(QPen(QBrush(borderColor), maskStroke));
    p.setBrush(Qt::NoBrush);
    p.drawRect(maskRect);
    //绘制边框上的拖动圆点
    if (isMouseup) {
        p.setBrush(borderColor);
        p.setPen(Qt::NoPen);
        auto r{ 1.5 * maskStroke };
        auto hw{ maskRect.width() / 2 };
        auto hh{ maskRect.height() / 2 };
        p.drawEllipse(maskRect.topLeft(), r, r);
        p.drawEllipse(QPointF(maskRect.left()+ hw, maskRect.top()), r, r);
        p.drawEllipse(maskRect.topRight(), r, r);
        p.drawEllipse(QPointF(maskRect.right(), maskRect.top()+ hh), r, r);
        p.drawEllipse(maskRect.bottomRight(), r, r);
        p.drawEllipse(QPointF(maskRect.left() + hw, maskRect.bottom()), r, r);
        p.drawEllipse(maskRect.bottomLeft(), r, r);
        p.drawEllipse(QPointF(maskRect.left(), maskRect.top() + hh), r, r);
    }
    paint();
    p.end();
    if (isMouseup) {
        releaseImg();
    }
}
void WinMask::changeMaskRect(const QPoint& pos)
{
    if (mousePosState == 1)
    {
        maskRect.setTopLeft(pos);
    }
    else if (mousePosState == 2)
    {
        maskRect.setTop(pos.y());
    }
    else if (mousePosState == 3)
    {
        maskRect.setTopRight(pos);
    }
    else if (mousePosState == 4)
    {
        maskRect.setRight(pos.x());
    }
    else if (mousePosState == 5)
    {
        maskRect.setBottomRight(pos);
    }
    else if (mousePosState == 6)
    {
        maskRect.setBottom(pos.y());
    }
    else if (mousePosState == 7)
    {
        maskRect.setBottomLeft(pos);
    }
    else if (mousePosState == 8)
    {
        maskRect.setLeft(pos.x());
    }
    maskRect = maskRect.normalized();
    update();
}
void WinMask::changeMousePosState(const int& x, const int& y)
{
    auto leftX = maskRect.topLeft().x(); auto topY = maskRect.topLeft().y();
    auto rightX = maskRect.bottomRight().x(); auto bottomY = maskRect.bottomRight().y();
    if (maskRect.contains(x, y))
    {
        QGuiApplication::setOverrideCursor(Qt::SizeAllCursor);
        mousePosState = 0;
    }
    else if (x < leftX && y < topY)
    {
        QGuiApplication::setOverrideCursor(Qt::SizeFDiagCursor);
        mousePosState = 1;
    }
    else if (x >= leftX && x < rightX && y < topY)
    {
        QGuiApplication::setOverrideCursor(Qt::SizeVerCursor);
        mousePosState = 2;
    }
    else if (x >= rightX && y < topY)
    {
        QGuiApplication::setOverrideCursor(Qt::SizeBDiagCursor);
        mousePosState = 3;
    }
    else if (x >= rightX && y >= topY && y < bottomY)
    {
        QGuiApplication::setOverrideCursor(Qt::SizeHorCursor);
        mousePosState = 4;
    }
    else if (x >= rightX && y >= bottomY)
    {
        QGuiApplication::setOverrideCursor(Qt::SizeFDiagCursor);
        mousePosState = 5;
    }
    else if (x >= leftX && x < rightX && y >= bottomY)
    {
        QGuiApplication::setOverrideCursor(Qt::SizeVerCursor);
        mousePosState = 6;
    }
    else if (x < leftX && y >= bottomY)
    {
        QGuiApplication::setOverrideCursor(Qt::SizeBDiagCursor);
        mousePosState = 7;
    }
    else if (x < leftX && y < bottomY && y >= topY)
    {
        QGuiApplication::setOverrideCursor(Qt::SizeHorCursor);
        mousePosState = 8;
    }
}
void WinMask::changeMousePosState2(const int& x, const int& y)
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

void WinMask::initWinRects()
{
    EnumWindows([](HWND hwnd, LPARAM lparam)
        {
            if (!hwnd) return TRUE;
            if (!IsWindowVisible(hwnd)) return TRUE;
            if (IsIconic(hwnd)) return TRUE;
            if (GetWindowTextLength(hwnd) < 1) return TRUE;
            auto self = (WinMask*)lparam;
            if (hwnd == (HWND)self->hwnd) return TRUE;
            auto father = (WinFull*)self->parent();
            if (hwnd == father->hwnd) return TRUE;
            RECT rect;
            DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &rect, sizeof(RECT));
            if (rect.right - rect.left <= 6 || rect.bottom - rect.top <= 6) {
                return TRUE;
            }
            self->winNativeRects.push_back(QRect(QPoint(rect.left-self->x, rect.top-self->y), QPoint(rect.right-self->x, rect.bottom-self->y)));
            self->winHwnds.push_back(hwnd);
            return TRUE;
        }, (LPARAM)this);
}


