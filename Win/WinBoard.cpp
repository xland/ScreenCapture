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
    img = QImage(w, h, QImage::Format_ARGB32_Premultiplied);
    img.fill(Qt::transparent);
    QPainter painter(&img);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);
    auto winFull = (WinFull*)parent();
    for (auto shape : winFull->shapes)
    {
        if (shape->isEraser || shape->state == ShapeState::ready) {
            shape->paint(&painter);
        }
    }
    paint();
    painter.end();
    releaseImg();
}
