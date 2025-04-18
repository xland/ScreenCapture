#include <qpainter.h>
#include <QTransform>

#include "ShapeArrow.h"
#include "../App/App.h"
#include "../Tool/ToolSub.h"
#include "../Win/WinBase.h"
#include "../Win/WinCanvas.h"

ShapeArrow::ShapeArrow(QObject* parent) : ShapeBase(parent)
{
    auto win = (WinBase*)parent;
    isFill = win->toolSub->getSelectState("arrowFill");
    color = win->toolSub->getColor();
    arrowSize = win->toolSub->getStrokeWidth();
}

ShapeArrow::~ShapeArrow()
{
}

void ShapeArrow::resetDragger()
{
    prepareDraggers(2);
    auto half{ draggerSize / 2 };    
    draggers[0].setRect(shape[0].x() - half, shape[0].y() - half, draggerSize, draggerSize);
    draggers[1].setRect(shape[3].x() - half, shape[3].y() - half, draggerSize, draggerSize);
}

void ShapeArrow::resetShape()
{
    QLineF line(startPos, endPos);
    qreal length = line.length();
    auto angle = line.angle();
    qreal v1{ (qreal)arrowSize / 4.f };
    qreal v2{ (qreal)arrowSize / 3.f*2.f };
    shape.clear();
    shape.append(QPointF(0, 0));
    shape.append(QPointF(length - arrowSize, -v1));
    shape.append(QPointF(length - arrowSize - v1, -v2));
    shape.append(QPointF(length, 0));
    shape.append(QPointF(length - arrowSize - v1, v2));
    shape.append(QPointF(length - arrowSize, v1));
    shape.append(QPointF(0, 0));
    QTransform transform;
    transform.translate(startPos.x(), startPos.y());
    transform.rotate(-angle);
    shape = transform.map(shape);
}

void ShapeArrow::paint(QPainter* painter)
{
    if (isFill) {
        painter->setBrush(QBrush(color));
        painter->setPen(Qt::NoPen);
    }
    else {
        painter->setPen(QPen(QBrush(color), 1));
        painter->setBrush(Qt::NoBrush);
    }
    painter->drawPolygon(shape);
}
void ShapeArrow::paintDragger(QPainter* painter)
{
    painter->setPen(QPen(QBrush(QColor(0, 0, 0)), 1));
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(draggers[0]);
    painter->drawRect(draggers[1]);
}
bool ShapeArrow::mouseMove(QMouseEvent* event)
{
    if (state != ShapeState::ready) return true;
    auto pos = event->pos();
    hoverDraggerIndex = -1;
    if (draggers[0].contains(pos)) {
        hoverDraggerIndex = 0;
    }
    else if (draggers[1].contains(pos)) {
        hoverDraggerIndex = 1;
    } 
    else if (shape.containsPoint(event->pos(), Qt::WindingFill)) {
        hoverDraggerIndex = 8;
    }
    if (hoverDraggerIndex > -1) {
        QGuiApplication::setOverrideCursor(Qt::SizeAllCursor);
        showDragger();
        event->accept();
    }
    return false;
}
bool ShapeArrow::mousePress(QMouseEvent* event)
{
    if (state == ShapeState::temp) {
        startPos = event->position();
        state = (ShapeState)((int)ShapeState::sizing0 + 1);
        paintingPrepare();
        return true;
    }
    else if(hoverDraggerIndex >= 0) {
        pressPos = event->position();
        state = (ShapeState)((int)ShapeState::sizing0 + hoverDraggerIndex);
        paintingStart();
        return true;
    }
    return false;
}
void ShapeArrow::mouseRelease(QMouseEvent* event)
{
    if (shape.isEmpty()) { //鼠标按下，没有拖拽，随即释放
        deleteLater();
        event->accept();
        return;
    }
    if (state >= ShapeState::sizing0) {
        resetDragger();
        showDragger();
        event->accept();
    }
}
void ShapeArrow::mouseDrag(QMouseEvent* event)
{
    if (state == ShapeState::ready) {
        return;
    }
    if (state == ShapeState::sizing0) {
        startPos = event->pos();
        resetShape();
    }
    else if (state == ShapeState::sizing1) {
        endPos = event->pos();
        resetShape();
    }
    else if (state == ShapeState::moving) {
        auto pos = event->pos();
        auto span = pos - pressPos;
        shape.translate(span);
        startPos = shape[0];
        endPos = shape[3];
        pressPos = pos;
    }
    painting();
    event->accept();
    //if (event->modifiers() & Qt::ShiftModifier) {
    //    if (shape.width() > shape.height()) {
    //        shape.setHeight(shape.width());
    //    }
    //    else {
    //        shape.setWidth(shape.height());
    //    }
    //}
}
