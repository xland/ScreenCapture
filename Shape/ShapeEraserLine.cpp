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
    painter->save();
    painter->setCompositionMode(QPainter::CompositionMode_Clear);
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

