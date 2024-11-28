#include <QEvent>
#include <Windows.h>
#include <QWindow>

#include "WinBase.h"
#include "WinCanvas.h"
#include "WinBoard.h"
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


WinBase::WinBase(QWidget *parent, Qt::WindowFlags flags) : QWidget(parent,flags)
{
}

WinBase::~WinBase()
{
    delete winCanvas;
}

ShapeBase* WinBase::addShape()
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

void WinBase::mousePressOnShape(QMouseEvent* event)
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

void WinBase::mouseMoveOnShape(QMouseEvent* event)
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

void WinBase::mouseDragOnShape(QMouseEvent* event)
{
    if (event->isAccepted()) return;
    for (int i = shapes.size() - 1; i >= 0; i--)
    {
        if (event->isAccepted()) return;
        shapes[i]->mouseDrag(event);
    }
}

void WinBase::mouseReleaseOnShape(QMouseEvent* event)
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
void WinBase::initWindow()
{
    setAutoFillBackground(false);
    setMouseTracking(true);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_NoSystemBackground);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool); //   | Qt::WindowStaysOnTopHint
    setFixedSize(w, h);
    show();
    SetWindowPos((HWND)winId(), nullptr, x, y, w, h, SWP_NOZORDER | SWP_SHOWWINDOW);
    auto dpr = windowHandle()->devicePixelRatio();
    img.setDevicePixelRatio(dpr);
}
void WinBase::updateCursor(Qt::CursorShape cur)
{
    if (cursor().shape() != cur) {
        setCursor(cur);
    }
}
void WinBase::refreshBoard()
{
    winBoard->refresh();
}
void WinBase::refreshCanvas(ShapeBase* shape,bool force)
{
    winCanvas->refresh(shape, force);
}
