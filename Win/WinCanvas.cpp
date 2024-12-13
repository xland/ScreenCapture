#include <QPainter>
#include <Windows.h>

#include "WinCanvas.h"
#include "WinBase.h"
#include "WinFull.h"
#include "../Shape/ShapeBase.h"

WinCanvas::WinCanvas(QObject *parent) : WinBase(parent)
{
    auto winFull = (WinFull*)parent;
    initSizeByWin(winFull);
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
    p->setRenderHint(QPainter::Antialiasing, true);
    p->setRenderHint(QPainter::TextAntialiasing, true);
    return p;
}
