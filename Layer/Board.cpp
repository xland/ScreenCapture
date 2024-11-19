#include <QPainter>
#include "Board.h"
#include "../Shape/ShapeBase.h"
#include "../Win/WinBase.h"

Board::Board(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);
    setAutoFillBackground(false);
    setAttribute(Qt::WA_TransparentForMouseEvents, true);
    setGeometry(parent->rect());
    lower();
    show();
}

Board::~Board()
{
}

void Board::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);

    painter.setBrush(QBrush(Qt::red));
    painter.setPen(Qt::NoPen);
    painter.drawRect(100, 100, 500, 500);
    painter.setCompositionMode(QPainter::CompositionMode_Clear);
    painter.drawRect(200, 200, 500, 500);

    //auto win = (WinBase*)parent();
    //for (auto shape:win->shapes)
    //{
    //    if (shape->state == ShapeState::ready) {
    //        shape->paint(&painter);
    //    }
    //}
}
