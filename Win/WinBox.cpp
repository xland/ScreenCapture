#include "WinBox.h"
#include "WinCanvas.h"

#include "WinFull.h"
#include "../App/Util.h"
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
#include "../Tool/ToolMain.h"
#include "PixelInfo.h"

WinBox::WinBox(QWidget* parent) : WinBase(parent)
{
}

WinBox::~WinBox()
{
}

ShapeBase* WinBox::addShape()
{
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
    connect(shape, &QObject::destroyed, [this](QObject* obj) { 
        shapes.removeOne(obj); //先执行析构函数，再执行此方法
    });
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
            QGuiApplication::setOverrideCursor(Qt::IBeamCursor);
        }
        else {
            QGuiApplication::setOverrideCursor(Qt::CrossCursor);
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
    //if (winCanvas && !event->isAccepted()) {
    //    if (state != State::text) {
    //        winCanvas->refresh(nullptr);
    //    }
    //}
}
void WinBox::removeShape()
{
    if (winCanvas->curShape) {
        shapes.removeOne(winCanvas->curShape);
		winCanvas->curShape->deleteLater();
        //winBoard->refresh();
        winCanvas->clear();
    }    
}
void WinBox::undo()
{
    for (int i = shapes.size() - 1; i >= 0; i--)
    {
        if (shapes[i]->state != ShapeState::hidden) {
			shapes[i]->state = ShapeState::hidden;
			//winBoard->refresh();
            winCanvas->clear();
			break;
        }
    }
}
void WinBox::redo()
{
    for (int i = 0; i <shapes.size(); i++)
    {
        if (shapes[i]->state == ShapeState::hidden) {
            shapes[i]->state = ShapeState::ready;
            //winBoard->refresh();
            winCanvas->clear();
            break;
        }
    }
}
void WinBox::hideTools(State state)
{
    if (toolMain) {
        toolMain->hide();
    }
    if (toolSub) {
        toolSub->hide();
    }
    if (winCanvas->curShape) {
        winCanvas->clear();
    }
	this->state = state;
    if (state == State::start) {
        if (!pixelInfo) {
            pixelInfo = new PixelInfo(this);
        }
    }
}


