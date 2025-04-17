
#include "../Win/WinBase.h"
#include "../Tool/ToolSub.h"
#include "../Shape/ShapeBase.h"
#include "../Shape/ShapeRect.h"
#include "../Shape/ShapeEllipse.h"
#include "../Shape/ShapeArrow.h"
#include "../Shape/ShapeNumber.h"
#include "../Shape/ShapeLine.h"
#include "../Shape/ShapeText.h"
#include "../Shape/ShapeEraserRect.h"
#include "../Shape/ShapeEraserLine.h"
#include "../Shape/ShapeMosaicRect.h"
#include "../Shape/ShapeMosaicLine.h"
#include "Canvas.h"


Canvas::Canvas(QObject *parent) : QObject(parent)
{

}

Canvas::~Canvas()
{

}

void Canvas::mousePress(QMouseEvent* event)
{
    auto shape = addShape();
    shape->mousePress(event); //不然新添加的Shape收不到鼠标按下事件
}

void Canvas::mouseDrag(QMouseEvent* event)
{
    for (int i = shapes.size() - 1; i >= 0; i--)
    {
        shapes[i]->mouseDrag(event);
    }
}

void Canvas::mouseRelease(QMouseEvent* event)
{
    for (int i = shapes.size() - 1; i >= 0; i--)
    {
        shapes[i]->mouseRelease(event);
    }
}

void Canvas::mouseMove(QMouseEvent* event)
{
    for (int i = shapes.size() - 1; i >= 0; i--)
    {
        shapes[i]->mouseMove(event);
    }
}

void Canvas::paint(QPainter& p)
{
}

ShapeBase* Canvas::addShape()
{
    ShapeBase* shape;
    auto win = (WinBase*)parent();
    if (win->state == State::rect) {
        shape = new ShapeRect(this);
    }
    else if (win->state == State::ellipse) {
        shape = new ShapeEllipse(this);
    }
    else if (win->state == State::arrow) {
        shape = new ShapeArrow(this);
    }
    else if (win->state == State::number) {
        shape = new ShapeNumber(this);
    }
    else if (win->state == State::line) {
        shape = new ShapeLine(this);
    }
    else if (win->state == State::text) {
        shape = new ShapeText(this);
    }
    else if (win->state == State::mosaic) {
        auto isRect = win->toolSub->getSelectState("mosaicFill");
        if (isRect) {
            shape = new ShapeMosaicRect(this);
        }
        else {
            shape = new ShapeMosaicLine(this);
        }
    }
    else if (win->state == State::eraser) {
        auto isRect = win->toolSub->getSelectState("eraserFill");
        if (isRect) {
            shape = new ShapeEraserRect(this);
        }
        else {
            shape = new ShapeEraserLine(this);
        }
    }
    else
    {
        return nullptr;
    }
    connect(shape, &QObject::destroyed, [this](QObject* obj) {
        shapes.removeOne(obj); //先执行析构函数，再执行此方法
        });
    shapes.push_back(shape);
    return shape;
}