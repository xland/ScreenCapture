#include <qpainter.h>
#include <QTransform>
#include <numbers>

#include "ShapeEraserRect.h"
#include "../App/App.h"
#include "../Tool/ToolSub.h"
#include "../Win/WinBase.h"
#include "../Win/Canvas.h"


ShapeEraserRect::ShapeEraserRect(QObject* parent) : ShapeRectBase(parent)
{
}

ShapeEraserRect::~ShapeEraserRect()
{
}

void ShapeEraserRect::paint(QPainter* painter)
{
    if (shape.isEmpty()) return;
    painter->setPen(Qt::NoPen);
    if (state == ShapeState::ready) {
        painter->save();
        painter->setCompositionMode(QPainter::CompositionMode_Clear); //橡皮擦模式
        painter->setBrush(Qt::transparent);
    }
    else {
        painter->setBrush(QColor(220, 80, 0, 120));
    }
    painter->drawRect(shape);
    if (state == ShapeState::ready) {
        painter->restore();
    }
}

void ShapeEraserRect::paintDragger(QPainter* painter)
{
    ShapeRectBase::paintDragger(painter);
	QPen pen;
	pen.setColor(Qt::black);
	pen.setWidth(1);
    pen.setStyle(Qt::CustomDashLine);
    pen.setDashPattern({ 3,3 });
	painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
	painter->drawRect(shape);
}
