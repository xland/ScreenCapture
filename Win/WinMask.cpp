#include <QWindow>
#include <dwmapi.h>
#include <shellscalingapi.h>

#include "WinMask.h"
#include "../Win/WinBase.h"
#include "../Tool/ToolMain.h"
#include "../Tool/ToolSub.h"
#include "../Tool/PixelInfo.h"

WinMask::WinMask(QWidget* parent) : WinBaseLayer(parent)
{
}

WinMask::~WinMask()
{
}

void WinMask::mousePress(QMouseEvent* event)
{
    if (father->state == State::start)
    {
        posPress = getNativeWinPos();
        father->state = State::mask;
        father->pixelInfo->hide();
        event->accept();
        return;
    }
    if (father->state == State::tool)
    {
        posPress = getNativeWinPos();
        father->toolMain->hide();
        if (mousePosState != 0)
        {
            changeMaskRect(posPress);
        }
        event->accept();
        return;
    }
    if (father->state > State::tool && mousePosState > 0) {
        posPress = getNativeWinPos();
        father->toolMain->hide();
        father->toolSub->hide();
        event->accept();
        return;
    }
}

void WinMask::mouseDrag(QMouseEvent* event)
{
    if (father->state == State::mask)
    {
        auto pos = getNativeWinPos();
        maskRect.setCoords(posPress.x(), posPress.y(), pos.x(),pos.y());
        maskRect = maskRect.normalized();
        update();
        return;
    }
    if (father->state == State::tool)
    {
        if (mousePosState == 0)
        {
            auto pos = getNativeWinPos();
            auto span = pos - posPress;
            maskRect.moveTo(maskRect.topLeft() + span);
            posPress = pos;
            update();
        }
        else
        {
            auto pos = getNativeWinPos();
            changeMaskRect(pos);
        }
        return;
    }
    if (father->state > State::tool && mousePosState > 0) {
        auto pos = getNativeWinPos();
        changeMaskRect(pos);
        return;
    }
}

void WinMask::mouseRelease(QMouseEvent* event)
{
    if (father->state == State::mask)
    {
        father->state = State::tool;
        father->showToolMain();
    }
    else if (father->state == State::tool)
    {
        father->showToolMain();
    }
    if (father->state > State::tool && mousePosState > 0) {
        father->showToolMain();
        father->showToolSub();
        return;
    }
}

void WinMask::mouseMove(QMouseEvent* event)
{
    if (father->state == State::start)
    {
        event->accept();    
        for (int i = 0; i < winNativeRects.size(); i++)
        {
            if (winNativeRects[i].contains(getNativePos())) {
                if (mouseInRectIndex == i) return;
                mouseInRectIndex = i;
                auto& winRect = winNativeRects[i];
                QPoint lt(winNativeRects[i].left()-father->x, winNativeRects[i].top()-father->y);
                QPoint rb(winNativeRects[i].right()-father->x, winNativeRects[i].bottom()-father->y);
                maskRect = QRectF(lt, rb);
                update();
                return;
            }
        }
    }
    else if (father->state == State::tool)
    {
        auto pos = getNativeWinPos();
        changeMousePosState(pos.x(), pos.y());
        event->accept();
    }
    else if (father->state > State::tool) {
        auto pos = getNativeWinPos();
        changeMousePosState2(pos.x(), pos.y());
        if (mousePosState != -1) {
            event->accept();
            return;
        }
    }
}
void WinMask::paintEvent(QPaintEvent* event)
{
    img.setDevicePixelRatio(1.0);
    img.fill(QColor(0, 0, 0, 120));
    QPainter p(&img);
    p.setRenderHint(QPainter::Antialiasing, true);
    auto sw = maskStroke * father->dpr;
    p.setPen(QPen(QBrush(QColor(22, 119, 255)), sw));
    p.setBrush(Qt::NoBrush);
    p.drawRect(maskRect.adjusted(-sw, -sw, sw, sw));
    p.setCompositionMode(QPainter::CompositionMode_Clear);
    p.setBrush(Qt::transparent);
    p.drawRect(maskRect);
    QPainter painter(this);
    img.setDevicePixelRatio(father->dpr);
    painter.drawImage(0,0,img);
}
void WinMask::ready()
{
    img = father->img.copy();
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
        father->updateCursor(Qt::SizeAllCursor);
        mousePosState = 0;
    }
    else if (x < leftX && y < topY)
    {
        father->updateCursor(Qt::SizeFDiagCursor);
        mousePosState = 1;
    }
    else if (x >= leftX && x < rightX && y < topY)
    {
        father->updateCursor(Qt::SizeVerCursor);
        mousePosState = 2;
    }
    else if (x >= rightX && y < topY)
    {
        father->updateCursor(Qt::SizeBDiagCursor);
        mousePosState = 3;
    }
    else if (x >= rightX && y >= topY && y < bottomY)
    {
        father->updateCursor(Qt::SizeHorCursor);
        mousePosState = 4;
    }
    else if (x >= rightX && y >= bottomY)
    {
        father->updateCursor(Qt::SizeFDiagCursor);
        mousePosState = 5;
    }
    else if (x >= leftX && x < rightX && y >= bottomY)
    {
        father->updateCursor(Qt::SizeVerCursor);
        mousePosState = 6;
    }
    else if (x < leftX && y >= bottomY)
    {
        father->updateCursor(Qt::SizeBDiagCursor);
        mousePosState = 7;
    }
    else if (x < leftX && y < bottomY && y >= topY)
    {
        father->updateCursor(Qt::SizeHorCursor);
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
        father->updateCursor(Qt::SizeFDiagCursor);
        mousePosState = 1;
    }
    else if (x >= x2 && x <= x3 && y >= y1 && y <= y2) {
        father->updateCursor(Qt::SizeVerCursor);
        mousePosState = 2;
    }
    else if (x >= x3 && x <= x4 && y >= y1 && y <= y2)
    {
        father->updateCursor(Qt::SizeBDiagCursor);
        mousePosState = 3;
    }
    else if (x >= x3 && x <= x4 && y >= y2 && y <= y3)
    {
        father->updateCursor(Qt::SizeHorCursor);
        mousePosState = 4;
    }
    else if (x >= x3 && x <= x4 && y >= y3 && y <= y4)
    {
        father->updateCursor(Qt::SizeFDiagCursor);
        mousePosState = 5;
    }
    else if (x >= x2 && x <= x3 && y >= y3 && y <= y4)
    {
        father->updateCursor(Qt::SizeVerCursor);
        mousePosState = 6;
    }
    else if (x >= x1 && x <= x2 && y >= y3 && y <= y4)
    {
        father->updateCursor(Qt::SizeBDiagCursor);
        mousePosState = 7;
    }
    else if (x >= x1 && x <= x2 && y >= y2 && y <= y3)
    {
        father->updateCursor(Qt::SizeHorCursor);
        mousePosState = 8;
    }
    else {
        mousePosState = -1;
    }
}
QPoint WinMask::getNativePos()
{
    POINT p;
    GetCursorPos(&p);
    return QPoint(p.x,p.y);
}
QPoint WinMask::getNativeWinPos()
{
    POINT p;
    GetCursorPos(&p);
    return QPoint(p.x-father->x, p.y-father->y);
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
            if (hwnd == (HWND)self->winId()) return TRUE;
            if (hwnd == (HWND)self->father->winId()) return TRUE;
            RECT rect;
            DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &rect, sizeof(RECT));
            if (rect.right - rect.left <= 6 || rect.bottom - rect.top <= 6) {
                return TRUE;
            }
            self->winNativeRects.push_back(QRect(QPoint(rect.left, rect.top), QPoint(rect.right, rect.bottom)));
            self->winHwnds.push_back(hwnd);
            return TRUE;
        }, (LPARAM)this);
}


