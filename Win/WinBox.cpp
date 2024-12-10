#include "WinBox.h"
#include "WinCanvas.h"
#include "WinBoard.h"

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
#include "../Tool/ToolSub.h"

WinBox::WinBox(QObject* parent) : WinBase(parent)
{
}

WinBox::~WinBox()
{
}

ShapeBase* WinBox::addShape()
{
    for (auto it = shapes.begin(); it != shapes.end(); ) {
        if ((*it)->state == ShapeState::temp) {
            (*it)->deleteLater();
            it = shapes.erase(it);
        }
        else {
            ++it;
        }
    }
    ShapeBase* shape;
    if (state == State::rect) {
        shape = new ShapeRect(this);
    }
    else if (state == State::ellipse) {
        shape = new ShapeEllipse(this);
    }
    else if (state == State::arrow) {
        shape = new ShapeArrow(this);
    }
    else if (state == State::number) {
        shape = new ShapeNumber(this);
    }
    else if (state == State::line) {
        shape = new ShapeLine(this);
    }
    else if (state == State::text) {
        shape = new ShapeText(this);
    }
    else if (state == State::mosaic) {
        auto isRect = toolSub->getSelectState("mosaicFill");
        if (isRect) {
            shape = new ShapeMosaicRect(this);
        }
        else {
            shape = new ShapeMosaicLine(this);
        }
    }
    else if (state == State::eraser) {
        auto isRect = toolSub->getSelectState("eraserFill");
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
    shapes.push_back(shape);
    return shape;
}
void WinBox::mousePressOnShape(QMouseEvent* event)
{
    if (event->isAccepted()) return;
    for (int i = shapes.size() - 1; i >= 0; i--)
    {
        if (event->isAccepted()) return;
        shapes[i]->mousePress(event);
    }
    if (!event->isAccepted()) {
        auto shape = addShape();
        shape->mousePress(event); //不然新添加的Shape收不到鼠标按下事件
    }
}

void WinBox::mouseMoveOnShape(QMouseEvent* event)
{
    if (event->isAccepted()) return;
    for (int i = shapes.size() - 1; i >= 0; i--)
    {
        if (event->isAccepted()) return;
        shapes[i]->mouseMove(event);
    }
    if (!event->isAccepted()) {
        if (state == State::text) {
            updateCursor(Qt::IBeamCursor);
        }
        else {
            updateCursor(Qt::CrossCursor);
        }
        if (winCanvas) {
            winCanvas->refresh(nullptr);
        }
    }
}

void WinBox::mouseDragOnShape(QMouseEvent* event)
{
    if (event->isAccepted()) return;
    for (int i = shapes.size() - 1; i >= 0; i--)
    {
        if (event->isAccepted()) return;
        shapes[i]->mouseDrag(event);
    }
}

void WinBox::mouseReleaseOnShape(QMouseEvent* event)
{
    if (event->isAccepted()) return;
    for (int i = shapes.size() - 1; i >= 0; i--)
    {
        if (event->isAccepted()) return;
        shapes[i]->mouseRelease(event);
    }
    if (winCanvas && !event->isAccepted()) {
        if (state != State::text) {
            winCanvas->refresh(nullptr);
        }
    }
}
void WinBox::updateCursor(const Qt::CursorShape& cur)
{
    QGuiApplication::setOverrideCursor(cur);
    //QGuiApplication::overrideCursor()
    //if (cursor().shape() != cur) {
    //    setCursor(cur);
    //}
}
void WinBox::refreshBoard()
{
    winBoard->refresh();
}
void WinBox::refreshCanvas(ShapeBase* shape, bool force)
{
    winCanvas->refresh(shape, force);
}
