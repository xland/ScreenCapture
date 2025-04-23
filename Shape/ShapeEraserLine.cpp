#include <qpainter.h>
#include <QTransform>
#include <numbers>

#include "ShapeEraserLine.h"
#include "../App/App.h"
#include "../Tool/ToolSub.h"
#include "../Win/WinBase.h"
#include "../Win/Canvas.h"


ShapeEraserLine::ShapeEraserLine(QObject* parent) : ShapeLineBase(parent)
{
    isEraser = true;
}

ShapeEraserLine::~ShapeEraserLine()
{
}

void ShapeEraserLine::paint(QPainter* painter)
{
	if (path.isEmpty()) return;
    painter->setPen(Qt::NoPen);
    if (state == ShapeState::ready) {
        painter->save();
        painter->setCompositionMode(QPainter::CompositionMode_Clear); //橡皮擦模式
        painter->setBrush(Qt::transparent);
    }
    else {
        painter->setBrush(QColor(220, 80, 0, 120));
    }
    QPainterPath path1 = stroker.createStroke(path);
    painter->drawPath(path1);
    if (state == ShapeState::ready) {
        painter->restore();
    }
}

