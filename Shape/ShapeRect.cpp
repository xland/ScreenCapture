#include <qpainter.h>

#include "ShapeRect.h"
#include "../App/App.h"
#include "../Tool/ToolSub.h"
#include "../Win/WinBase.h"
#include "../Win/WinCanvas.h"

ShapeRect::ShapeRect(QObject* parent) : ShapeRectBase(parent)
{
    auto win = (WinBase*)parent;
    isFill = win->toolSub->getSelectState("rectFill");
    strokeWidth = win->toolSub->getStrokeWidth();
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