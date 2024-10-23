#include <qcursor.h>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsScene>

#include "CutMask.h"
#include "CanvasWidget.h"
#include "Tool/ToolMain.h"
#include "WindowNative.h"
#include "Shape/ShapeBase.h"
#include "Shape/ShapeDragger.h"
#include "Shape/ShapeRect.h"
#include "Shape/ShapeEllipse.h"


CutMask::CutMask() : QGraphicsPathItem()
{
    auto win = WindowNative::Get();
    winRect.setRect(-maskStroke * 6, -maskStroke * 6, win->w + maskStroke * 6, win->h + maskStroke * 6);
    maskRect.setRect(-maskStroke, -maskStroke, win->w + maskStroke, win->h + maskStroke);
    updatePath();
    setAcceptHoverEvents(true);
    //setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
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
        changeMousePosState(event->pos().x(), event->pos().y());
        return;
    }
    if (!isShapeUseMouse) {
        changeMousePosState2(event->pos().x(), event->pos().y());
    }
    emit hoverMove(event);
    //canvas->shapeDragger->hoverMove(event);
    //if (event->isAccepted()) {
    //    return;
    //}
    //auto items = scene()->items();
    //for (size_t i = 2; i < items.size(); i++)
    //{
    //    dynamic_cast<ShapeBase*>(items[i])->hoverMove(event);
    //    if (event->isAccepted()) {
    //        return;
    //    }
    //}
    //setCursor(Qt::CrossCursor);
}

void CutMask::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    dragPosition = event->pos();
    auto canvasWidget = CanvasWidget::Get();
    if (canvasWidget->state == State::start)
    {
        canvasWidget->state = State::mask;
        maskRect.setRect(dragPosition.x(), dragPosition.y(), 0, 0);
        return;
    }
    if (canvasWidget->state == State::tool)
    {
        canvasWidget->toolMain->hide();
        if (mousePosState != 0)
        {
            changeMaskRect(dragPosition);
        }
        return;
    }
    if (!isShapeUseMouse) {
        addShape(canvasWidget->state);
    }
    emit mousePress(event);
    //auto items = scene()->items();
    //for (size_t i = 2; i < items.size(); i++)
    //{
    //    dynamic_cast<ShapeBase*>(items[i])->mousePress(event);
    //}
    //if (!event->isAccepted()) {
    //    canvasWidget->addShape();
    //}
}

void CutMask::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    auto canvasWidget = CanvasWidget::Get();
    auto pos = event->pos();
    if (canvasWidget->state == State::mask)
    {
        maskRect.setBottomRight(pos);
        updatePath();
        return;
    }
    if (canvasWidget->state == State::tool)
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
        return;
    }
    emit mouseMove(event);
    //auto items = scene()->items();
    //for (size_t i = 2; i < items.size(); i++)
    //{
    //    dynamic_cast<ShapeBase*>(items[i])->mouseMove(event);
    //}
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
     emit mouseRelease(event);
     //auto items = scene()->items();
     //for (size_t i = 2; i < items.size(); i++)
     //{
     //    dynamic_cast<ShapeBase*>(items[i])->mouseRelease(event);
     //}
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

void CutMask::changeMousePosState(const qreal& x, const qreal& y)
{
    auto leftX = maskRect.topLeft().x(); auto topY = maskRect.topLeft().y();
    auto rightX = maskRect.bottomRight().x(); auto bottomY = maskRect.bottomRight().y();
    if (maskRect.contains(x,y))
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

void CutMask::changeMousePosState2(const qreal& x, const qreal& y)
{
    auto x1{ maskRect.x() - maskStroke}, x2{ x1 + maskStroke * 3 };
    auto x3{ maskRect.right() - maskStroke }, x4{ x3 + maskStroke*3 };
    auto y1{ maskRect.y() - maskStroke }, y2{ y1 + maskStroke *3 };
    auto y3{ maskRect.bottom() - maskStroke }, y4{ y3 + maskStroke*3 };
    if (x >= x1 && x<=x2 && y>=y1 && y <= y2) {
        setCursor(Qt::SizeFDiagCursor);
        mousePosState = 1;
    }
    else if (x >= x2 && x<=x3 && y>=y1 && y <= y2) {
        setCursor(Qt::SizeVerCursor);
        mousePosState = 2;
    }
    else if (x >= x3 && x <= x4 && y >= y1 && y <= y2)
    {
        setCursor(Qt::SizeBDiagCursor);
        mousePosState = 3;
    }
    else if (x >= x3 && x <= x4 && y >= y2 && y <= y3)
    {
        setCursor(Qt::SizeHorCursor);
        mousePosState = 4;
    }
    else if (x >= x3 && x <= x4 && y >= y3 && y <= y4)
    {
        setCursor(Qt::SizeFDiagCursor);
        mousePosState = 5;
    }
    else if (x >= x2 && x <= x3 && y >= y3 && y <= y4)
    {
        setCursor(Qt::SizeVerCursor);
        mousePosState = 6;
    }
    else if (x >= x1 && x <= x2 && y >= y3 && y <= y4)
    {
        setCursor(Qt::SizeBDiagCursor);
        mousePosState = 7;
    }
    else if (x >= x1 && x <= x2 && y >= y2 && y <= y3)
    {
        setCursor(Qt::SizeHorCursor);
        mousePosState = 8;
    }
    else {
        setCursor(Qt::CrossCursor);
    }
}

void CutMask::updatePath()
{
    p.clear();
    p.addRect(winRect);
    p.addRect(maskRect);
    setPath(p);
}

void CutMask::addShape(const State& state)
{
    if (state == State::rect) {
        scene()->addItem(new ShapeRect());
    }
}
