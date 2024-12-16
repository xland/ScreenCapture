#include <qpainter.h>
#include <QTransform>
#include <numbers>

#include "ShapeEraserLine.h"
#include "../App/App.h"
#include "../Tool/ToolSub.h"
#include "../Win/WinBox.h"
#include "../Win/WinBoard.h"
#include "../Win/WinCanvas.h"


ShapeEraserLine::ShapeEraserLine(QObject* parent) : ShapeLineBase(parent)
{
    isEraser = true;
}

ShapeEraserLine::~ShapeEraserLine()
{
}

void ShapeEraserLine::paint(QPainter* painter)
{
    QPen pen(Qt::transparent);
    pen.setWidth(strokeWidth);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    painter->save();
    painter->setCompositionMode(QPainter::CompositionMode_Clear);
    if (path.isEmpty()) {
        painter->drawLine(startPos, endPos);
    }
    else {
        painter->drawPath(path);
    }
    painter->restore();
}

void ShapeEraserLine::paintDragger(QPainter* painter)
{
    QPen pen;
    pen.setColor(Qt::black);
    pen.setWidth(1);
    painter->setBrush(Qt::NoBrush);
    QLineF line(startPos, endPos);
    auto length = line.length();
    auto angle = line.angle();
    auto half = strokeWidth / 2;
    QPolygonF borderShape;
    borderShape.append(QPointF(-half, -half));
    borderShape.append(QPointF(length + half, -half));
    borderShape.append(QPointF(length + half, half));
    borderShape.append(QPointF(-half, half));
    borderShape.append(QPointF(-half, -half));
    QTransform transform;
    transform.translate(startPos.x(), startPos.y());
    transform.rotate(-angle);
    borderShape = transform.map(borderShape);
    pen.setStyle(Qt::CustomDashLine);  //为橡皮擦区域画虚线
    pen.setDashPattern({ 3,3 });
    painter->setPen(pen);
    painter->drawPolygon(borderShape);
}

void ShapeEraserLine::paintingStart()
{
    auto win = (WinBox*)parent();
    win->winBoard->initImg(); //鼠标按下，初始化图像
    win->winCanvas->clear();  //清空虚线框
}

void ShapeEraserLine::paintOnBoard()
{
    auto win = (WinBox*)parent();
    win->winBoard->refresh(false);
    win->winBoard->releaseImg(); //绘制完成，清空内存
}

void ShapeEraserLine::painting()
{
    auto win = (WinBox*)parent();
    win->winBoard->refresh(false); //绘制过程，不断刷新，刷新时不释放图像
}

