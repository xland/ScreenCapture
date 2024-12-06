#include <qpainter.h>
#include <QTransform>
#include <numbers>

#include "ShapeEraserLine.h"
#include "../App/App.h"
#include "../Tool/ToolSub.h"
#include "../Win/WinBase.h"
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

