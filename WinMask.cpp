#include <qcursor.h>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsScene>

#include "App.h"
#include "WinMask.h"
#include "WinBoard.h"
#include "Tool/ToolMain.h"
#include "Tool/ToolSub.h"
#include "WinFull.h"
#include "Shape/ShapeBase.h"
#include "Shape/ShapeDragger.h"
#include "Shape/ShapeRect.h"
#include "Shape/ShapeEllipse.h"

namespace {
    WinMask* winMask;
}

WinMask::WinMask(QWidget* parent) : QWidget(parent)
{
    //必须得让Board接收鼠标事件，因为Mask的透明区域没办法处理鼠标事件
    setAttribute(Qt::WA_TransparentForMouseEvents, true);
}

WinMask::~WinMask()
{
}

void WinMask::paintEvent(QPaintEvent* event)
{
    auto full = App::getFull();
    if (!full) return;
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setPen(QPen(QBrush(QColor(22, 119, 255)), maskStroke));
    painter.setBrush(QBrush(QColor(0, 0, 0, 120)));
    p.clear();
    p.addRect(-16, -16, full->w + 16, full->h +16);
    p.addRect(maskRect);
    painter.drawPath(p);
}

void WinMask::showEvent(QShowEvent* event)
{
}

void WinMask::mousePress(QMouseEvent* event)
{
    dragPosition = event->pos();
    auto full = App::getFull();
    if (full->state == State::start)
    {
        full->state = State::mask;
        maskRect.setRect(dragPosition.x(), dragPosition.y(), 0, 0);
        event->accept();
        return;
    }
    if (full->state == State::tool)
    {
        full->toolMain->hide();
        if (mousePosState != 0)
        {
            changeMaskRect(dragPosition);
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
void WinMask::mouseMove(QMouseEvent* event)
{
    auto full = App::getFull();
    auto pos = event->pos();
    if (full->state == State::tool)
    {
        changeMousePosState(pos.x(), pos.y());
        event->accept();
        return;
    }
    if (full->state > State::tool) {
        changeMousePosState2(pos.x(), pos.y());
        if (mousePosState != -1) {
            event->accept();
            return;
        }
    }
    setCursor(Qt::CrossCursor);
}
void WinMask::mouseDrag(QMouseEvent* event) {
    auto full = App::getFull();
    auto pos = event->pos();
    if (full->state == State::mask)
    {
        maskRect.setBottomRight(pos);
        update();
        return;
    }
    if (full->state == State::tool)
    {
        if (mousePosState == 0)
        {
            auto span = pos - dragPosition;
            maskRect.moveTo(maskRect.topLeft() + span);
            dragPosition = pos;
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
     auto full = App::getFull();
     if (full->state == State::mask)
     {
         full->state = State::tool;
         full->toolMain->show();
     }
     else if (full->state == State::tool)
     {
         full->toolMain->show();
     }
     if (full->state > State::tool && mousePosState > 0) {
         full->toolMain->show();
         full->toolSub->show();
         return;
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
    update();
}

void WinMask::changeMousePosState(const int& x, const int& y)
{
    auto leftX = maskRect.topLeft().x(); auto topY = maskRect.topLeft().y();
    auto rightX = maskRect.bottomRight().x(); auto bottomY = maskRect.bottomRight().y();
    auto board = App::getFull()->board;
    if (maskRect.contains(x,y))
    {
        board->setCursor(Qt::SizeAllCursor);
        mousePosState = 0;
    }
    else if (x < leftX && y < topY)
    {
        board->setCursor(Qt::SizeFDiagCursor);
        mousePosState = 1;
    }
    else if (x >= leftX && x < rightX && y < topY)
    {
        board->setCursor(Qt::SizeVerCursor);
        mousePosState = 2;
    }
    else if (x >= rightX && y < topY)
    {
        board->setCursor(Qt::SizeBDiagCursor);
        mousePosState = 3;
    }
    else if (x >= rightX && y >= topY && y < bottomY)
    {
        board->setCursor(Qt::SizeHorCursor);
        mousePosState = 4;
    }
    else if (x >= rightX && y >= bottomY)
    {
        board->setCursor(Qt::SizeFDiagCursor);
        mousePosState = 5;
    }
    else if (x >= leftX && x < rightX && y >= bottomY)
    {
        board->setCursor(Qt::SizeVerCursor);
        mousePosState = 6;
    }
    else if (x < leftX && y >= bottomY)
    {
        board->setCursor(Qt::SizeBDiagCursor);
        mousePosState = 7;
    }
    else if (x < leftX && y < bottomY && y >= topY)
    {
        board->setCursor(Qt::SizeHorCursor);
        mousePosState = 8;
    }
}

void WinMask::changeMousePosState2(const int& x, const int& y)
{
    auto x1{ maskRect.x() - maskStroke}, x2{ x1 + maskStroke * 3 };
    auto x3{ maskRect.right() - maskStroke }, x4{ x3 + maskStroke*3 };
    auto y1{ maskRect.y() - maskStroke }, y2{ y1 + maskStroke *3 };
    auto y3{ maskRect.bottom() - maskStroke }, y4{ y3 + maskStroke*3 };
    auto board = App::getFull()->board;
    if (x >= x1 && x<=x2 && y>=y1 && y <= y2) {
        board->setCursor(Qt::SizeFDiagCursor);
        mousePosState = 1;
    }
    else if (x >= x2 && x<=x3 && y>=y1 && y <= y2) {
        board->setCursor(Qt::SizeVerCursor);
        mousePosState = 2;
    }
    else if (x >= x3 && x <= x4 && y >= y1 && y <= y2)
    {
        board->setCursor(Qt::SizeBDiagCursor);
        mousePosState = 3;
    }
    else if (x >= x3 && x <= x4 && y >= y2 && y <= y3)
    {
        board->setCursor(Qt::SizeHorCursor);
        mousePosState = 4;
    }
    else if (x >= x3 && x <= x4 && y >= y3 && y <= y4)
    {
        board->setCursor(Qt::SizeFDiagCursor);
        mousePosState = 5;
    }
    else if (x >= x2 && x <= x3 && y >= y3 && y <= y4)
    {
        board->setCursor(Qt::SizeVerCursor);
        mousePosState = 6;
    }
    else if (x >= x1 && x <= x2 && y >= y3 && y <= y4)
    {
        board->setCursor(Qt::SizeBDiagCursor);
        mousePosState = 7;
    }
    else if (x >= x1 && x <= x2 && y >= y2 && y <= y3)
    {
        board->setCursor(Qt::SizeHorCursor);
        mousePosState = 8;
    }
    else {
        board->setCursor(Qt::CrossCursor);
        mousePosState = -1;
    }
}