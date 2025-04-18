
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
    for (int i = shapes.size() - 1; i >= 0; i--)
    {
        auto flag = shapes[i]->mousePress(event);
        if (flag) {
            return;
        }
    }
    addShape();
    shapeCur->mousePress(event);
}

void Canvas::mouseDrag(QMouseEvent* event)
{
    if (shapeCur) {
        shapeCur->mouseDrag(event);
    }
}

void Canvas::mouseRelease(QMouseEvent* event)
{
    if (shapeCur) {
        auto win = (WinBase*)parent();
        QPainter p(&win->imgBoard);
        p.setRenderHint(QPainter::Antialiasing, true);
        shapeCur->mouseRelease(event);
        shapeCur->paint(&p);
        win->update();
    }
}

void Canvas::mouseMove(QMouseEvent* event)
{
    for (int i = shapes.size() - 1; i >= 0; i--)
    {
        auto flag = shapes[i]->mouseMove(event);
        if (flag) {
            return;
        }
    }
}

void Canvas::paint(QPainter& p)
{
    if (shapeCur) {
        shapeCur->paint(&p);
    }
    if (shapeHover) {
        shapeHover->paintDragger(&p);
    }
}

void Canvas::addShape()
{
    auto win = (WinBase*)parent();
    if (win->state == State::rect) {
        shapeCur = new ShapeRect(win);
    }
    else if (win->state == State::ellipse) {
        shapeCur = new ShapeEllipse(win);
    }
    else if (win->state == State::arrow) {
        shapeCur = new ShapeArrow(win);
    }
    else if (win->state == State::number) {
        shapeCur = new ShapeNumber(win);
    }
    else if (win->state == State::line) {
        shapeCur = new ShapeLine(win);
    }
    else if (win->state == State::text) {
        shapeCur = new ShapeText(win);
    }
    else if (win->state == State::mosaic) {
        auto isRect = win->toolSub->getSelectState("mosaicFill");
        if (isRect) {
            shapeCur = new ShapeMosaicRect(win);
        }
        else {
            shapeCur = new ShapeMosaicLine(win);
        }
    }
    else if (win->state == State::eraser) {
        auto isRect = win->toolSub->getSelectState("eraserFill");
        if (isRect) {
            shapeCur = new ShapeEraserRect(win);
        }
        else {
            shapeCur = new ShapeEraserLine(win);
        }
    }
    else
    {
        return;
    }
    //connect(shape, &QObject::destroyed, [this](QObject* obj) {
    //    shapes.removeOne(obj); //先执行析构函数，再执行此方法
    //    });
    shapes.push_back(shapeCur);
}

void Canvas::setHoverShape(ShapeBase* shape)
{
    if (!timerDragger) {
        timerDragger = new QTimer(this);
        timerDragger->setInterval(800);
        timerDragger->setSingleShot(true);
        connect(timerDragger, &QTimer::timeout, [this]() {
            if (shapeHover->hoverDraggerIndex != -1) {
                timerDragger->start();
            }
            else {
                shapeHover = nullptr;
                auto win = (WinBase*)parent();
                win->update();
            }
            });
    }
    if (shape != shapeHover) {
        shapeHover = shape;
        auto win = (WinBase*)parent();
        win->update();
    }
    timerDragger->start();
}

void Canvas::removeShapeFromBoard(ShapeBase* shape)
{
    auto win = (WinBase*)parent();
    win->imgBoard.fill(Qt::transparent);
    QPainter p(&win->imgBoard);
    p.setRenderHint(QPainter::Antialiasing, true);
    for (auto& s : shapes)
    {
        if (shape == s) continue;
        s->paint(&p);
    }
}
