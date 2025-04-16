#include <qpainter.h>
#include <QTransform>
#include <numbers>

#include "ShapeEraserRect.h"
#include "../App/App.h"
#include "../Tool/ToolSub.h"
#include "../Win/WinBox.h"
#include "../Win/WinCanvas.h"


ShapeEraserRect::ShapeEraserRect(QObject* parent) : ShapeRectBase(parent)
{
    isFill = true;
    isEraser = true;
}

ShapeEraserRect::~ShapeEraserRect()
{
}

void ShapeEraserRect::paint(QPainter* painter)
{
    painter->setBrush(Qt::transparent);
    painter->setPen(Qt::NoPen);
    painter->save();
    painter->setCompositionMode(QPainter::CompositionMode_Clear);
    painter->drawRect(shape);
    painter->restore();

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

void ShapeEraserRect::paintingStart()
{
    //auto win = (WinBox*)parent();
    //win->winBoard->initImg(); //鼠标按下，初始化图像
    //win->winCanvas->clear();  //清空dragger
}

void ShapeEraserRect::paintOnBoard()
{
    //auto win = (WinBox*)parent();
    //win->winBoard->refresh(false);
    //win->winBoard->releaseImg(); //绘制完成，清空内存
}

void ShapeEraserRect::painting()
{
    //auto win = (WinBox*)parent();
    //win->winBoard->refresh(false); //绘制过程，不断刷新，刷新时不释放图像
}
