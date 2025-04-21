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
    painter->save();
    painter->setCompositionMode(QPainter::CompositionMode_Clear); //橡皮擦模式
    QPainterPath path1 = stroker.createStroke(path);
    painter->setPen(Qt::NoPen);
    painter->setBrush(Qt::transparent);
    painter->drawPath(path1);
    if (state == ShapeState::sizing0) {
        QPainterPath path2 = stroker.createStroke(pathStart);
        painter->drawPath(path2);
    }
    painter->restore();
}

