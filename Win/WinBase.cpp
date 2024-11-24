#include <QEvent>
#include <Windows.h>

#include "WinBase.h"
#include "WinCanvas.h"
#include "../Shape/ShapeBase.h"
#include "../Shape/ShapeRect.h"
#include "../Shape/ShapeEllipse.h"
#include "../Shape/ShapeArrow.h"
#include "../Shape/ShapeNumber.h"
#include "../Shape/ShapeLine.h"
#include "../Shape/ShapeText.h"
#include "../Shape/ShapeEraser.h"
#include "../Shape/ShapeMosaic.h"


WinBase::WinBase(QWidget *parent) : QWidget(parent)
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
        shape = new ShapeMosaic(this);
    }
    else if (state == State::eraser) {
        shape = new ShapeEraser(this);
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
            winCanvas->changeShape(nullptr);
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
            winCanvas->changeShape(nullptr);
        }
    }
}
void WinBase::paintShape(QPainter* painter)
{
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter->setRenderHint(QPainter::TextAntialiasing, true);
    for (auto shape : shapes)
    {
        if (shape->state == ShapeState::ready) {
            shape->paint(painter);
        }
    }
}
void WinBase::initWindow()
{
    setAutoFillBackground(false);
    setMouseTracking(true);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_NoSystemBackground);
    setWindowFlags(Qt::FramelessWindowHint); //  | Qt::Tool | Qt::WindowStaysOnTopHint
    setFixedSize(w, h);
    show();
    SetWindowPos((HWND)winId(), nullptr, x, y, w, h, SWP_NOZORDER | SWP_SHOWWINDOW);
}
void WinBase::updateCursor(Qt::CursorShape cur)
{
    if (cursor().shape() != cur) {
        setCursor(cur);
    }
}