#include <QPainter>
#include <Windows.h>


#include "WinBoard.h"
#include "WinBase.h"
#include "WinFull.h"
#include "../Shape/ShapeBase.h"

WinBoard::WinBoard(QObject *parent) : WinBase(parent)
{
    auto winFull = (WinFull*)parent;
    initSizeByWin(winFull);
    initWindow();
    show();
}

WinBoard::~WinBoard()
{
}

void WinBoard::refresh()
{
    update();
}

void WinBoard::update()
{
    if (img.isNull()) {
        img = QImage(w, h, QImage::Format_ARGB32);
    }
    img.fill(Qt::transparent);
    QPainter painter(&img);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);
    auto winFull = (WinFull*)parent();
    for (auto shape : winFull->shapes)
    {
        if (shape->isEraser || shape->state == ShapeState::ready) {
            shape->paint(&painter);
        }
    }
    paint();
}
