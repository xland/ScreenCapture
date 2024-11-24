#include <QWindow>
#include <dwmapi.h>

#include "WinMask.h"
#include "../Win/WinBase.h"
#include "../Win/WinFull.h"
#include "../Tool/ToolMain.h"
#include "../Tool/ToolSub.h"

WinMask::WinMask(QWidget* parent) : QWidget(parent)
{
    initWinRects();
    initWindow();
}

WinMask::~WinMask()
{
}
void WinMask::initWindow()
{
    auto winBase = (WinBase*)WinFull::get();
    setAutoFillBackground(false);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setAttribute(Qt::WA_QuitOnClose, false);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAttribute(Qt::WA_TransparentForMouseEvents, true);
    setFixedSize(winBase->w, winBase->h);
    show();
    auto hwnd = (HWND)winId();
    SetWindowPos(hwnd, nullptr, winBase->x, winBase->y, winBase->w, winBase->h, SWP_NOZORDER | SWP_SHOWWINDOW);
    LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
    SetWindowLong(hwnd, GWL_EXSTYLE, exStyle | WS_EX_TRANSPARENT);
}

void WinMask::mousePress(QMouseEvent* event)
{
    posPress = event->pos();
    auto full = WinFull::get();
    if (full->state == State::start)
    {
        full->state = State::mask;
        event->accept();
        return;
    }
    if (full->state == State::tool)
    {
        full->toolMain->hide();
        if (mousePosState != 0)
        {
            changeMaskRect(posPress);
        }
        event->accept();
        return;
    }
    if (full->state > State::tool && mousePosState > 0) {
        full->toolMain->hide();
        full->toolSub->hide();
        event->accept();
        return;
    }
}

void WinMask::mouseDrag(QMouseEvent* event)
{
    auto full = WinFull::get();
    auto pos = event->pos();
    if (full->state == State::mask)
    {
        maskRect.setCoords(posPress.x(), posPress.y(), pos.x(),pos.y());
        update();
        return;
    }
    if (full->state == State::tool)
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
    if (full->state > State::tool && mousePosState > 0) {
        changeMaskRect(pos);
        return;
    }
}

void WinMask::mouseRelease(QMouseEvent* event)
{
    maskRect = maskRect.normalized();
    auto full = WinFull::get();
    if (full->state == State::mask)
    {
        full->state = State::tool;
        full->showToolMain();
    }
    else if (full->state == State::tool)
    {
        full->showToolMain();
    }
    if (full->state > State::tool && mousePosState > 0) {
        full->showToolMain();
        full->showToolSub();
        return;
    }
}

void WinMask::mouseMove(QMouseEvent* event)
{
    auto pos = event->pos();
    auto full = WinFull::get();
    if (full->state == State::start)
    {
        for (int i = 0; i < winRects.size(); i++)
        {
            if (winRects[i].contains(pos)) {
                if (maskRect != winRects[i]) {
                    maskRect = winRects[i];
                    update();
                }
                break;
            }
        }
        event->accept();
    }
    else if (full->state == State::tool)
    {
        changeMousePosState(pos.x(), pos.y());
        event->accept();
    }
    else if (full->state > State::tool) {
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
    auto winBase = (WinBase*)WinFull::get();
    painter.setPen(QPen(QBrush(QColor(22, 119, 255)), maskStroke));
    painter.setBrush(QBrush(QColor(0, 0, 0, 120)));
    p.clear();
    p.addRect(-16, -16, winBase->w + 16, winBase->h + 16);
    p.addRect(maskRect);
    painter.drawPath(p);
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
    WinFull::get()->refresh();
}
void WinMask::changeMousePosState(const int& x, const int& y)
{
    auto leftX = maskRect.topLeft().x(); auto topY = maskRect.topLeft().y();
    auto rightX = maskRect.bottomRight().x(); auto bottomY = maskRect.bottomRight().y();
    auto winFull = WinFull::get();
    if (maskRect.contains(x, y))
    {
        winFull->updateCursor(Qt::SizeAllCursor);
        mousePosState = 0;
    }
    else if (x < leftX && y < topY)
    {
        winFull->updateCursor(Qt::SizeFDiagCursor);
        mousePosState = 1;
    }
    else if (x >= leftX && x < rightX && y < topY)
    {
        winFull->updateCursor(Qt::SizeVerCursor);
        mousePosState = 2;
    }
    else if (x >= rightX && y < topY)
    {
        winFull->updateCursor(Qt::SizeBDiagCursor);
        mousePosState = 3;
    }
    else if (x >= rightX && y >= topY && y < bottomY)
    {
        winFull->updateCursor(Qt::SizeHorCursor);
        mousePosState = 4;
    }
    else if (x >= rightX && y >= bottomY)
    {
        winFull->updateCursor(Qt::SizeFDiagCursor);
        mousePosState = 5;
    }
    else if (x >= leftX && x < rightX && y >= bottomY)
    {
        winFull->updateCursor(Qt::SizeVerCursor);
        mousePosState = 6;
    }
    else if (x < leftX && y >= bottomY)
    {
        winFull->updateCursor(Qt::SizeBDiagCursor);
        mousePosState = 7;
    }
    else if (x < leftX && y < bottomY && y >= topY)
    {
        winFull->updateCursor(Qt::SizeHorCursor);
        mousePosState = 8;
    }
}
void WinMask::changeMousePosState2(const int& x, const int& y)
{
    auto x1{ maskRect.x() - maskStroke }, x2{ x1 + maskStroke * 3 };
    auto x3{ maskRect.right() - maskStroke }, x4{ x3 + maskStroke * 3 };
    auto y1{ maskRect.y() - maskStroke }, y2{ y1 + maskStroke * 3 };
    auto y3{ maskRect.bottom() - maskStroke }, y4{ y3 + maskStroke * 3 };
    auto winFull = WinFull::get();
    if (x >= x1 && x <= x2 && y >= y1 && y <= y2) {
        winFull->updateCursor(Qt::SizeFDiagCursor);
        mousePosState = 1;
    }
    else if (x >= x2 && x <= x3 && y >= y1 && y <= y2) {
        winFull->updateCursor(Qt::SizeVerCursor);
        mousePosState = 2;
    }
    else if (x >= x3 && x <= x4 && y >= y1 && y <= y2)
    {
        winFull->updateCursor(Qt::SizeBDiagCursor);
        mousePosState = 3;
    }
    else if (x >= x3 && x <= x4 && y >= y2 && y <= y3)
    {
        winFull->updateCursor(Qt::SizeHorCursor);
        mousePosState = 4;
    }
    else if (x >= x3 && x <= x4 && y >= y3 && y <= y4)
    {
        winFull->updateCursor(Qt::SizeFDiagCursor);
        mousePosState = 5;
    }
    else if (x >= x2 && x <= x3 && y >= y3 && y <= y4)
    {
        winFull->updateCursor(Qt::SizeVerCursor);
        mousePosState = 6;
    }
    else if (x >= x1 && x <= x2 && y >= y3 && y <= y4)
    {
        winFull->updateCursor(Qt::SizeBDiagCursor);
        mousePosState = 7;
    }
    else if (x >= x1 && x <= x2 && y >= y2 && y <= y3)
    {
        winFull->updateCursor(Qt::SizeHorCursor);
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
            auto winBase = (WinBase*)WinFull::get();
            if (hwnd == (HWND)self->winId()) return TRUE;
            if (hwnd == (HWND)winBase->winId()) return TRUE;
            RECT rect;
            DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &rect, sizeof(RECT));
            if (rect.right - rect.left <= 6 || rect.bottom - rect.top <= 6) {
                return TRUE;
            }
            auto sf = winBase->windowHandle()->devicePixelRatio();
            auto l{ (rect.left - winBase->x) / sf },
                t{ (rect.top - winBase->y) / sf },
                r{ (rect.right - winBase->x) / sf },
                b{ (rect.bottom - winBase->y) / sf };
            self->winRects.push_back(QRect(QPoint(l, t), QPoint(r, b)));
            return TRUE;
        }, (LPARAM)this);
}


