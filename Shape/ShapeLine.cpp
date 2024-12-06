#include <qpainter.h>
#include <QTransform>
#include <numbers>

#include "ShapeLine.h"
#include "../App/App.h"
#include "../Tool/ToolSub.h"
#include "../Win/WinBase.h"
#include "../Win/WinCanvas.h"


ShapeLine::ShapeLine(QObject* parent) : ShapeLineBase(parent)
{
    auto win = (WinBase*)parent;
    auto isTransparent = win->toolSub->getSelectState("lineTransparent");
    color = win->toolSub->getColor();
    if (isTransparent) {
        color.setAlpha(128);
    }
}

ShapeLine::~ShapeLine()
{
}

void ShapeLine::paint(QPainter* painter)
{
    QPen pen(color);
    pen.setWidth(strokeWidth);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    if (path.isEmpty()) {
        painter->drawLine(startPos, endPos);
    }
    else {
        painter->drawPath(path);
    }

}

