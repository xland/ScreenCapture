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
    posPress = event->pos();
    if (father->state == State::start)
    {
        father->state = State::mask;
        father->pixelInfo->hide();
        event->accept();
        return;
    }
    if (father->state == State::tool)
    {
        father->toolMain->hide();
        if (mousePosState != 0)
        {
            changeMaskRect(posPress);
        }
        event->accept();
        return;
    }
    if (father->state > State::tool && mousePosState > 0) {
        father->toolMain->hide();
        father->toolSub->hide();
        event->accept();
        return;
    }
}

void WinMask::mouseDrag(QMouseEvent* event)
{
    auto pos = event->pos();
    if (father->state == State::mask)
    {
        maskRect.setCoords(posPress.x(), posPress.y(), pos.x(),pos.y());
        maskRect = maskRect.normalized();
        update();
        return;
    }
    if (father->state == State::tool)
    {
        if (mousePosState == 0)
        {
            auto span = pos - posPress;
            maskRect.moveTo(maskRect.topLeft() + span);
            posPress = pos;
            update();
        }
        else
        {
            changeMaskRect(pos);
        }
        return;
    }
    if (father->state > State::tool && mousePosState > 0) {
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
    auto pos = event->pos();
    if (father->state == State::start)
    {
        event->accept();
        POINT p;
        GetCursorPos(&p);
        for (int i = 0; i < winNativeRects.size(); i++)
        {
            if (winNativeRects[i].contains(p.x,p.y)) {
                if (mouseInRectIndex == i) return;
                mouseInRectIndex = i;
                //QPoint lt(winNativeRects[i].left() - father->x, winNativeRects[i].top() - father->y);
                //QPoint rb(winNativeRects[i].right() - father->x, winNativeRects[i].bottom() - father->y);
                QPoint lt(winNativeRects[i].left(), winNativeRects[i].top());
                QPoint rb(winNativeRects[i].right(), winNativeRects[i].bottom());
                lt = mapFromGlobal(lt);
                rb = mapFromGlobal(rb);
                QScreen* s;
                //s->mapBetween

                //auto rect = winNativeRects[i];
                //HMONITOR hMonitor = MonitorFromPoint({ rect.left(), rect.top()}, MONITOR_DEFAULTTONEAREST);
                //UINT dpiX = 0, dpiY = 0;
                //GetDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
                //auto dpr = dpiX / 96.0f;
                //auto l{ rect.left() / dpr }, t{ rect.top() / dpr };

                //hMonitor = MonitorFromPoint({ rect.right(), rect.bottom()}, MONITOR_DEFAULTTONEAREST);
                //GetDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
                //dpr = dpiX / 96.0f;
                //auto r{ rect.right() / dpr }, b{ rect.bottom() / dpr };
                //auto lt = QPoint(l, t);
                //auto rb = QPoint(r, b);
                //auto lt = mapFromGlobal(QPoint(l, t));
                //auto rb = mapFromGlobal(QPoint(r, b));
                //QRect(QPoint(l, t), QPoint(r, b))


                //注意，这里用的是主屏幕的dpr，不是窗口的dpr

                /*
                auto dpr = father->img.devicePixelRatio();
                int pArea;
                auto screens = QGuiApplication::screens();
                for (auto s : screens)
                {
                    auto geo = s->geometry();
                    auto pos = geo.topLeft();
                    if (pos.x() == 0 && pos.y() == 0)
                    {
                        auto pSize = s->size() * s->devicePixelRatio();                        
                        pArea = pSize.width() * pSize.height();
                        break;
                    }
                }
                for (auto s:screens)
                {
                    auto sSize = s->size() * s->devicePixelRatio();
                    auto sArea = sSize.width() * sSize.height();
                    if (pArea <= sArea) {
                        dpr = s->devicePixelRatio();
                        pArea = sArea;
                    }
                }*/

                //maskRect = QRectF(lt / dpr, rb / dpr);
                maskRect = QRectF(lt, rb);
                update();
                return;
            }
        }
    }
    else if (father->state == State::tool)
    {
        changeMousePosState(pos.x(), pos.y());
        event->accept();
    }
    else if (father->state > State::tool) {
        changeMousePosState2(pos.x(), pos.y());
        if (mousePosState != -1) {
            event->accept();
            return;
        }
    }
}
void WinMask::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::NoPen);
    painter.fillRect(rect(), QColor(0, 0, 0, 120));
    painter.setPen(QPen(QBrush(QColor(22, 119, 255)), maskStroke));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(maskRect.adjusted(-maskStroke,-maskStroke, maskStroke, maskStroke));
    painter.setCompositionMode(QPainter::CompositionMode_Clear);
    painter.setBrush(Qt::transparent);
    painter.drawRect(maskRect);
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
            return TRUE;
        }, (LPARAM)this);
}


