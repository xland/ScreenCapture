#include <qpen.h>
#include <qbrush.h>
#include <QGraphicsSceneHoverEvent>
#include "ShapeDragger.h"
#include "ShapeRect.h"
#include "../CanvasWidget.h"
#include "../CutMask.h"

ShapeDragger::ShapeDragger() : QGraphicsPathItem()
{
    setVisible(false);
    setAcceptHoverEvents(true);
    setPen(QPen(QBrush(QColor(0, 0, 0)), 1));
    setBrush(Qt::NoBrush);
    setZValue(886);
}
ShapeDragger::~ShapeDragger()
{
    
}

void ShapeDragger::ShowRectDragger(ShapeRect* shapeRect)
{
    p.clear();
    auto half{ shapeRect->strokeWidth / 2 };
    auto rect = shapeRect->shape.adjusted(-half, -half, half, half);
    auto x{ rect.x()};
    auto y{ rect.y() };
    auto w{ rect.width() };
    auto h{ rect.height() };

    rects.push_back(QRectF(x-size/2,y-size/2,size,size));
    rects.push_back(QRectF(x+w/2 - size / 2, y- size / 2, size, size));
    rects.push_back(QRectF(x + w - size / 2, y - size / 2, size, size));
    rects.push_back(QRectF(x + w - size / 2, y + h / 2 - size / 2, size, size));
    rects.push_back(QRectF(x + w - size / 2, y + h - size / 2, size, size));
    rects.push_back(QRectF(x + w/2 - size / 2, y + h - size / 2, size, size));
    rects.push_back(QRectF(x - size / 2, y + h - size / 2, size, size));
    rects.push_back(QRectF(x - size / 2, y + h/2 - size / 2, size, size));
    for (size_t i = 0; i < rects.size(); i++)
    {
        p.addRect(rects[i]);
    }
    setPath(p);
    show();
    state = State::rect;
}

void ShapeDragger::hoverMove(QGraphicsSceneHoverEvent* event)
{
    if (!isVisible()) {
        event->ignore();
        return;
    }
    if (state == State::rect) {
        if (hoverRectDragger(event)) {
            return;
        }
    }
    event->ignore();
}

void ShapeDragger::mouseMove(QGraphicsSceneMouseEvent* event)
{
}

void ShapeDragger::mousePress(QGraphicsSceneMouseEvent* event)
{
}

void ShapeDragger::mouseRelease(QGraphicsSceneMouseEvent* event)
{
}

bool ShapeDragger::hoverRectDragger(QGraphicsSceneHoverEvent* event)
{
    auto pos = event->pos();
    if (rects[0].contains(pos)|| rects[4].contains(pos)) {
        CanvasWidget::Get()->cutMask->setCursor(Qt::SizeFDiagCursor);
        event->accept();
        return true;
    }
    if (rects[1].contains(pos)|| rects[5].contains(pos)) {
        CanvasWidget::Get()->cutMask->setCursor(Qt::SizeVerCursor);
        event->accept();
        return true;
    }
    if (rects[2].contains(pos) || rects[6].contains(pos)) {
        CanvasWidget::Get()->cutMask->setCursor(Qt::SizeBDiagCursor);
        event->accept();
        return true;
    }
    if (rects[3].contains(pos) || rects[7].contains(pos)) {
        CanvasWidget::Get()->cutMask->setCursor(Qt::SizeHorCursor);
        event->accept();
        return true;
    }
    return false;
}
