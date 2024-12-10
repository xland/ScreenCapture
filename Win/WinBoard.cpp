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
    if (refreshFlag) return;
    refreshFlag = true;
    //update();
}

//void WinBoard::paintEvent(QPaintEvent* event)
//{
//    QPainter painter(this);
//    painter.setRenderHint(QPainter::Antialiasing, true);
//    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
//    painter.setRenderHint(QPainter::TextAntialiasing, true);
//    for (auto shape : father->shapes)
//    {
//        if (shape->isEraser || shape->state == ShapeState::ready) {
//            shape->paint(&painter);
//        }
//    }
//    refreshFlag = false;
//}
