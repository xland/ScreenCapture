#include <QPainter>
#include <Windows.h>

#include "WinCanvas.h"
#include "WinBase.h"
#include "WinBox.h"
#include "../Shape/ShapeBase.h"

WinCanvas::WinCanvas(QObject *parent) : WinBase(parent)
{
    auto win = (WinBox*)parent;
    x = win->x;
    y = win->y;
    w = win->w;
    h = win->h;
    initWindow();
    show();
}

WinCanvas::~WinCanvas()
{
}

void WinCanvas::paintShape()
{
    auto p = getPainter();
    curShape->paint(p.get());
    paint();
}

void WinCanvas::clear()
{
    curShape = nullptr;
    initImg();
    img.fill(Qt::transparent);
    paint();
    releaseImg();
}

void WinCanvas::paintDragger()
{
    initImg();
    auto p = getPainter();
    curShape->paintDragger(p.get());
    paint();
    p->end();
    releaseImg();
}

std::shared_ptr<QPainter> WinCanvas::getPainter()
{
    img.fill(Qt::transparent);
    auto p = std::make_shared<QPainter>(&img);
    auto win = (WinBox*)parent();
    if (win->padding > 0) {
        p->setClipRect(win->padding, win->padding, w - 2 * win->padding, h - 2 * win->padding);
    }
    p->setRenderHint(QPainter::Antialiasing, true);
    p->setRenderHint(QPainter::TextAntialiasing, true);
    return p;
}
