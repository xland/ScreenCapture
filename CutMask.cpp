#include <qcursor.h>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsScene>

#include "CutMask.h"
#include "CanvasWidget.h"
#include "Tool/ToolMain.h"
#include "WindowNative.h"
#include "Shape/ShapeBase.h"


CutMask::CutMask() : QGraphicsPathItem()
{
    auto win = WindowNative::Get();
    winRect.setRect(-maskStroke * 6, -maskStroke * 6, win->w + maskStroke * 6, win->h + maskStroke * 6);
    maskRect.setRect(-maskStroke, -maskStroke, win->w + maskStroke, win->h + maskStroke);
    updatePath();
    setAcceptHoverEvents(true);
    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
    setAcceptedMouseButtons(Qt::LeftButton);
    setPen(QPen(QBrush(QColor(22, 119, 255)), maskStroke));
    setBrush(QBrush(QColor(0, 0, 0, 120)));
    setZValue(888);
}

CutMask::~CutMask()
{
}

void CutMask::hoverMoveEvent(QGraphicsSceneHoverEvent* event)
{
    auto canvas = CanvasWidget::Get();
    if (canvas->state == State::tool)
    {
        changeMousePosState(event->pos());
    }
    else
    {
        setCursor(Qt::CrossCursor);
    }
    event->ignore();
    canvas->dispatchEvent(event);
}

void CutMask::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    dragPosition = event->pos();
    auto canvasWidget = CanvasWidget::Get();
    if (canvasWidget->state == State::start)
    {
        canvasWidget->state = State::mask;
        maskRect.setRect(dragPosition.x(), dragPosition.y(), 0, 0);
    }
    else if (canvasWidget->state == State::tool)
    {
        canvasWidget->toolMain->hide();
        if (mousePosState != 0)
        {
            changeMaskRect(dragPosition);
        }
    }
    auto items = scene()->items();
    for (size_t i = 1; i < items.size(); i++)
    {
        ShapeBase* shape = dynamic_cast<ShapeBase*>(items[i]);
        shape->mousePress(event);
    }
}

void CutMask::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    auto canvasWidget = CanvasWidget::Get();
    auto pos = event->pos();
    if (canvasWidget->state == State::mask)
    {
        maskRect.setBottomRight(pos);
        updatePath();
    }
    else if (canvasWidget->state == State::tool)
    {
        if (mousePosState == 0)
        {
            auto span = pos - dragPosition;
            maskRect.moveTo(maskRect.topLeft() + span);
            dragPosition = pos;
            updatePath();
        }
        else
        {
            changeMaskRect(pos);
        }
    }
    auto items = scene()->items();
    for (size_t i = 1; i < items.size(); i++)
    {
        ShapeBase* shape = dynamic_cast<ShapeBase*>(items[i]);
        shape->mouseMove(event);
    }
}

void CutMask::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
     maskRect = maskRect.normalized();    
     auto canvas = CanvasWidget::Get();
     if (canvas->state == State::mask)
     {
         canvas->state = State::tool;
         canvas->toolMain->show();
     }
     else if (canvas->state == State::tool)
     {
         canvas->toolMain->show();
     }
     auto items = scene()->items();
     for (size_t i = 1; i < items.size(); i++)
     {
         ShapeBase* shape = dynamic_cast<ShapeBase*>(items[i]);
         shape->mouseRelease(event);
     }
}

void CutMask::changeMaskRect(const QPointF& pos)
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
    updatePath();
}

void CutMask::changeMousePosState(const QPointF& pos)
{
    auto x = pos.x(); auto y = pos.y();
    auto leftX = maskRect.topLeft().x(); auto topY = maskRect.topLeft().y();
    auto rightX = maskRect.bottomRight().x(); auto bottomY = maskRect.bottomRight().y();
    if (maskRect.contains(pos))
    {
        setCursor(Qt::SizeAllCursor);
        mousePosState = 0;
    }
    else if (x < leftX && y < topY)
    {
        setCursor(Qt::SizeFDiagCursor);
        mousePosState = 1;
    }
    else if (x >= leftX && x < rightX && y < topY)
    {
        setCursor(Qt::SizeVerCursor);
        mousePosState = 2;
    }
    else if (x >= rightX && y < topY)
    {
        setCursor(Qt::SizeBDiagCursor);
        mousePosState = 3;
    }
    else if (x >= rightX && y >= topY && y < bottomY)
    {
        setCursor(Qt::SizeHorCursor);
        mousePosState = 4;
    }
    else if (x >= rightX && y >= bottomY)
    {
        setCursor(Qt::SizeFDiagCursor);
        mousePosState = 5;
    }
    else if (x >= leftX && x < rightX && y >= bottomY)
    {
        setCursor(Qt::SizeVerCursor);
        mousePosState = 6;
    }
    else if (x < leftX && y >= bottomY)
    {
        setCursor(Qt::SizeBDiagCursor);
        mousePosState = 7;
    }
    else if (x < leftX && y < bottomY && y >= topY)
    {
        setCursor(Qt::SizeHorCursor);
        mousePosState = 8;
    }
}

void CutMask::updatePath()
{
    p.clear();
    p.addRect(winRect);
    p.addRect(maskRect);
    setPath(p);
}
