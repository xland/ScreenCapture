#include <qpainter.h>
#include <QTransform>
#include <numbers>

#include "ShapeNumber.h"
#include "../App/App.h"
#include "../Tool/ToolSub.h"
#include "../Win/WinBase.h"
#include "../Win/Canvas.h"

namespace {
    static int numVal{ 0 };
}


ShapeNumber::ShapeNumber(QObject* parent) : ShapeBase(parent)
{
    auto win = (WinBase*)parent;
    prepareDraggers(3);
    isFill = win->toolSub->getSelectState("numberFill");
    color = win->toolSub->getColor();
    val = ++numVal;
}

ShapeNumber::~ShapeNumber()
{
}

void ShapeNumber::resetShape()
{
    shape.clear();
    auto x1{ r * qCos(qDegreesToRadians(12.0)) }, y1{ -r * qSin(qDegreesToRadians(12.0)) };
    shape.moveTo(x1,y1);
    QRectF rect(-r, -r, 2 * r, 2 * r);
    shape.arcTo(rect, 12, 338);
    shape.lineTo(r+r/3, 0); //三角形箭头的高度为r/3
    shape.lineTo(x1, y1);
    QTransform transform;
    transform.translate(centerPos.x(), centerPos.y());
    transform.rotate(-angle);
    shape = transform.map(shape);
    sizePos = transform.map(QPointF(-r,0));
    arrowPos = transform.map(QPointF(r + r / 3, 0));
}

void ShapeNumber::resetDraggers()
{
    auto half{ draggerSize / 2 };
    draggers[0].setRect(centerPos.x() - half, centerPos.y() - half, draggerSize, draggerSize);
    draggers[1].setRect(sizePos.x() - half, sizePos.y() - half, draggerSize, draggerSize);
    draggers[2].setRect(arrowPos.x() - half, arrowPos.y() - half, draggerSize, draggerSize);
}

void ShapeNumber::paint(QPainter* painter)
{
    if (isFill) {
        painter->setBrush(QBrush(color));
        painter->setPen(Qt::NoPen);
        painter->drawPath(shape);
        painter->setPen(QColor(255, 255, 255));
    }
    else {
        painter->setPen(QPen(QBrush(color), 1));
        painter->setBrush(Qt::NoBrush);
        painter->drawPath(shape);
        painter->setPen(color);
    }
    QRectF rect(centerPos.x() - r, centerPos.y() - r, 2 * r, 2 * r);
    auto font = painter->font();
    font.setPointSizeF(r / 4 * 3);
    painter->setFont(font);
    painter->drawText(rect, Qt::AlignCenter, QString::number(val));
}
void ShapeNumber::paintDragger(QPainter* painter)
{
    if (state != ShapeState::ready) return;
    painter->setPen(QPen(QBrush(QColor(0, 0, 0)), 1));
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(draggers[0]);
    painter->drawRect(draggers[1]);
    painter->drawRect(draggers[2]);
}
bool ShapeNumber::mouseMove(QMouseEvent* event)
{
    if (state != ShapeState::ready) return false;
    auto pos = event->pos();
    hoverDraggerIndex = -1;
    if (draggers[0].contains(pos)) {
        hoverDraggerIndex = 8; //移动
	}
	else if (draggers[1].contains(pos)) {
		hoverDraggerIndex = 0; //改变大小
	}
    else if(draggers[2].contains(pos)) {
        hoverDraggerIndex = 1; 
    }
    auto win = (WinBase*)parent();
    if (hoverDraggerIndex > -1) {
        win->canvas->setHoverShape(this);
        win->setCursor(Qt::SizeAllCursor);
    }
    else {
        return false;
    }
}
bool ShapeNumber::mousePress(QMouseEvent* event)
{
    if (state == ShapeState::temp) {
        centerPos = event->position();
        pressPos = centerPos;
        state = ShapeState::moving;
        resetShape();
        auto win = (WinBase*)parent();
        win->update();
        return true;
    }
    else if (hoverDraggerIndex >= 0) {
        if (state == ShapeState::ready) {
            auto win = (WinBase*)parent();
            win->canvas->removeShapeFromBoard(this);
        }
        pressPos = event->position();
        state = (ShapeState)((int)ShapeState::sizing0 + hoverDraggerIndex);
        return true;
    }
    return false;
}
bool ShapeNumber::mouseRelease(QMouseEvent* event)
{
    if (shape.isEmpty()) { //鼠标按下，没有拖拽，随即释放
        return false;
    }
    if (state >= ShapeState::sizing0) {
        resetDraggers();
        state = ShapeState::ready;
        auto win = (WinBase*)parent();
        win->canvas->setHoverShape(this);
        return true;
    }
    return false;
}

void ShapeNumber::mouseDrag(QMouseEvent* event)
{
    if (state == ShapeState::ready) {
        return;
    }
    if (state == ShapeState::sizing0) {
        QLineF line(centerPos, event->pos());
        r = line.length();
        resetShape();
    }
    else if (state == ShapeState::sizing1) {
        QLineF line(centerPos, event->pos());
        angle = line.angle();
        resetShape();
    }
    else if (state == ShapeState::moving) {
        auto pos = event->pos();
        auto span = pos - pressPos;
        shape.translate(span);
        centerPos += span;
        arrowPos += span;
        sizePos += span;
        pressPos = pos;
    }
    auto win = (WinBase*)parent();
    win->update();
}