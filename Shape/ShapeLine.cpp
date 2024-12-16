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
    QPen pen(Qt::red);
    pen.setWidth(1);
    pen.setCapStyle(Qt::RoundCap);
    painter->setPen(pen);
    painter->setBrush(color);
    painter->drawPath(path1);
    if (state == ShapeState::sizing0) {
        QPainterPath path2 = stroker.createStroke(pathStart);
        painter->drawPath(path2);
    }

    // 如果需要原始路径（作为对比）
    //painter->setPen(QPen(Qt::red, 2));  // 设置红色细线绘制原始路径
    //painter->setBrush(Qt::NoBrush);
    //painter->drawPath(path);


    //QPen pen(color);
    //pen.setWidth(strokeWidth);
    //pen.setCapStyle(Qt::RoundCap);
    //pen.setJoinStyle(Qt::RoundJoin);
    //painter->setPen(pen);
    //painter->setBrush(Qt::NoBrush);
    //if (path.elementCount() <= 0) {
    //    painter->drawLine(startPos, endPos);
    //}
    //else {
    //    painter->drawPath(path);
    //}

}

