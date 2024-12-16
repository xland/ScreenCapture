#include <qpainter.h>
#include <QTransform>
#include <numbers>

#include "ShapeLine.h"
#include "../App/App.h"
#include "../Tool/ToolSub.h"
#include "../Win/WinBox.h"
#include "../Win/WinCanvas.h"


ShapeLine::ShapeLine(QObject* parent) : ShapeLineBase(parent)
{
    auto win = (WinBox*)parent;
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
    QPainterPath path1 = stroker.createStroke(path);
    painter->setPen(Qt::NoPen);
    painter->setBrush(color);
    painter->drawPath(path1);
    if (state == ShapeState::sizing0) {
        QPainterPath path2 = stroker.createStroke(pathStart);
        painter->drawPath(path2);
    }
}

