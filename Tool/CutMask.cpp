

#include "CutMask.h"
#include "../Win/WinFull.h"

CutMask::CutMask(QWidget* parent) : QWidget(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents, true);
    setGeometry(parent->rect());
    show();
}

CutMask::~CutMask()
{
}

void CutMask::paintEvent(QPaintEvent* event)
{
    auto full = WinFull::get();
    QPainter painter(this);
    painter.setPen(QPen(QBrush(QColor(22, 119, 255)), maskStroke));
    painter.setBrush(QBrush(QColor(0, 0, 0, 120)));
    p.clear();
    p.addRect(-16, -16, full->w + 16, full->h + 16);
    p.addRect(maskRect);
    painter.drawPath(p);
}

void CutMask::showEvent(QShowEvent* event)
{
}

void CutMask::mousePress(QMouseEvent* event)
{
    posPress = event->pos();
    auto full = WinFull::get();
    if (full->state == State::start)
    {
        full->state = State::mask;        
        maskRect.setRect(posPress.x(), posPress.y(), 0, 0);
        event->accept();
        return;
    }
    //if (full->state == State::tool)
    //{
    //    full->toolMain->hide();
    //    if (mousePosState != 0)
    //    {
    //        changeMaskRect(dragPosition);
    //    }
    //    event->accept();
    //    return;
    //}
    //if (full->state > State::tool && mousePosState > 0) {
    //    full->toolMain->hide();
    //    full->toolSub->hide();
    //    event->accept();
    //    return;
    //}
}

void CutMask::mouseDrag(QMouseEvent* event)
{
    auto full = WinFull::get();
    auto pos = event->pos();
    if (full->state == State::mask)
    {
        maskRect.setBottomRight(pos);
        update();
        return;
    }
}

void CutMask::mouseRelease(QMouseEvent* event)
{
    maskRect = maskRect.normalized();
}

void CutMask::mouseMove(QMouseEvent* event)
{
    auto pos = event->pos();
    auto winFull = WinFull::get();
    if (winFull->state == State::start)
    {
        for (size_t i = 0; i < winFull->winRects.size(); i++)
        {
            if (winFull->winRects[i].contains(pos)) {
                if (maskRect != winFull->winRects[i]) {
                    maskRect = winFull->winRects[i];
                    update();
                }
                break;
            }
        }
        event->accept();
    }
    else if (winFull->state == State::tool)
    {
        changeMousePosState(pos.x(), pos.y());
        event->accept();
    }
    else if (winFull->state > State::tool) {
        changeMousePosState2(pos.x(), pos.y());
        if (mousePosState != -1) {
            event->accept();
            return;
        }
    }
    setCursor(Qt::CrossCursor);
}


void CutMask::changeMaskRect(const QPoint& pos)
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
    update();
}
void CutMask::changeMousePosState(const int& x, const int& y)
{
    auto leftX = maskRect.topLeft().x(); auto topY = maskRect.topLeft().y();
    auto rightX = maskRect.bottomRight().x(); auto bottomY = maskRect.bottomRight().y();
    auto winFull = WinFull::get();
    if (maskRect.contains(x, y))
    {
        winFull->setCursor(Qt::SizeAllCursor);
        mousePosState = 0;
    }
    else if (x < leftX && y < topY)
    {
        winFull->setCursor(Qt::SizeFDiagCursor);
        mousePosState = 1;
    }
    else if (x >= leftX && x < rightX && y < topY)
    {
        winFull->setCursor(Qt::SizeVerCursor);
        mousePosState = 2;
    }
    else if (x >= rightX && y < topY)
    {
        winFull->setCursor(Qt::SizeBDiagCursor);
        mousePosState = 3;
    }
    else if (x >= rightX && y >= topY && y < bottomY)
    {
        winFull->setCursor(Qt::SizeHorCursor);
        mousePosState = 4;
    }
    else if (x >= rightX && y >= bottomY)
    {
        winFull->setCursor(Qt::SizeFDiagCursor);
        mousePosState = 5;
    }
    else if (x >= leftX && x < rightX && y >= bottomY)
    {
        winFull->setCursor(Qt::SizeVerCursor);
        mousePosState = 6;
    }
    else if (x < leftX && y >= bottomY)
    {
        winFull->setCursor(Qt::SizeBDiagCursor);
        mousePosState = 7;
    }
    else if (x < leftX && y < bottomY && y >= topY)
    {
        winFull->setCursor(Qt::SizeHorCursor);
        mousePosState = 8;
    }
}
void CutMask::changeMousePosState2(const int& x, const int& y)
{
    auto x1{ maskRect.x() - maskStroke }, x2{ x1 + maskStroke * 3 };
    auto x3{ maskRect.right() - maskStroke }, x4{ x3 + maskStroke * 3 };
    auto y1{ maskRect.y() - maskStroke }, y2{ y1 + maskStroke * 3 };
    auto y3{ maskRect.bottom() - maskStroke }, y4{ y3 + maskStroke * 3 };
    auto winFull = WinFull::get();
    if (x >= x1 && x <= x2 && y >= y1 && y <= y2) {
        winFull->setCursor(Qt::SizeFDiagCursor);
        mousePosState = 1;
    }
    else if (x >= x2 && x <= x3 && y >= y1 && y <= y2) {
        winFull->setCursor(Qt::SizeVerCursor);
        mousePosState = 2;
    }
    else if (x >= x3 && x <= x4 && y >= y1 && y <= y2)
    {
        winFull->setCursor(Qt::SizeBDiagCursor);
        mousePosState = 3;
    }
    else if (x >= x3 && x <= x4 && y >= y2 && y <= y3)
    {
        winFull->setCursor(Qt::SizeHorCursor);
        mousePosState = 4;
    }
    else if (x >= x3 && x <= x4 && y >= y3 && y <= y4)
    {
        winFull->setCursor(Qt::SizeFDiagCursor);
        mousePosState = 5;
    }
    else if (x >= x2 && x <= x3 && y >= y3 && y <= y4)
    {
        winFull->setCursor(Qt::SizeVerCursor);
        mousePosState = 6;
    }
    else if (x >= x1 && x <= x2 && y >= y3 && y <= y4)
    {
        winFull->setCursor(Qt::SizeBDiagCursor);
        mousePosState = 7;
    }
    else if (x >= x1 && x <= x2 && y >= y2 && y <= y3)
    {
        winFull->setCursor(Qt::SizeHorCursor);
        mousePosState = 8;
    }
    else {
        winFull->setCursor(Qt::CrossCursor);
        mousePosState = -1;
    }
}