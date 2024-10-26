#include <qpainter.h>


#include "ShapeEllipse.h"
#include "../WinBoard.h"
#include "../Tool/ToolSub.h"

 ShapeEllipse::ShapeEllipse(QObject* parent) : ShapeBase(parent)
 {
 }

ShapeEllipse::~ShapeEllipse()
{
}

bool ShapeEllipse::contains(const QPoint& point)
{
    return false;
}

void ShapeEllipse::paint(QPainter* painter)
{

}

void ShapeEllipse::paintDragger(QPainter* painter)
{
}

void ShapeEllipse::mousePress(QMouseEvent* event)
{
    //if (isReady) {
    //    return;
    //}
    //rectShape.setTopLeft(event->pos());
    //auto toolSub = WinBoard::Get()->toolSub;
    //isFill = toolSub->getSelectState("rectFill");
    //color = toolSub->getColor();
    //strokeWidth = toolSub->getStrokeWidth();
    //if (isFill) {
    //    setBrush(QBrush(color));
    //    setPen(Qt::NoPen);
    //}
    //else {
    //    setPen(QPen(QBrush(color), strokeWidth));
    //    setBrush(Qt::NoBrush);
    //}
    event->ignore();
}

void ShapeEllipse::hoverMove(QMouseEvent* event)
{
}

void ShapeEllipse::mouseRelease(QMouseEvent* event)
{
    //if (isReady) {
    //    return;
    //}
    //isReady = true;
    //scene()->addItem(new ShapeEllipse());
}

void ShapeEllipse::mouseMove(QMouseEvent* event)
{
    //if (isReady) {
    //    return;
    //}
    //auto pos = event->pos();
    //rectShape.setBottomRight(pos);
    //setRect(rectShape);
    //scene()->invalidate(rectShape.adjusted(-56, -56, 56, 56));
}
