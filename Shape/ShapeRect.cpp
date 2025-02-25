#include <qpainter.h>

#include "ShapeRect.h"
#include "../App/App.h"
#include "../Tool/ToolSub.h"
#include "../Win/Box.h"
#include "../Win/Canvas.h"

ShapeRect::ShapeRect(QObject* parent) : ShapeRectBase(parent)
{
    isFill = box->toolSub->getSelectState("rectFill");    
    color = box->toolSub->getColor();
    strokeWidth = box->toolSub->getStrokeWidth();
}

ShapeRect::~ShapeRect()
{
}
void ShapeRect::paint(QPainter* painter)
{
    if (isFill) {
        painter->setBrush(QBrush(color));
        painter->setPen(Qt::NoPen);
    }
    else {
        painter->setPen(QPen(QBrush(color), strokeWidth));
        painter->setBrush(Qt::NoBrush);
    }
    painter->drawRect(shape);
}