

#include "LayerMask.h"
#include "../Win/WinFull.h"

LayerMask::LayerMask(QObject *parent) : LayerBase(parent)
{
 //   auto winFull = WinFull::get();
 //   img = winFull->grab().toImage(); //QImage(winFull->bgImg.size(), QImage::Format_ARGB32);
 //   img.fill(QColor(0, 0, 0, 0));
 //   painter.begin(&img);
 //   painter.setRenderHint(QPainter::Antialiasing, true);
 //   painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
 //   painter.setPen(QPen(QBrush(QColor(22, 119, 255)), maskStroke));
 //   painter.setBrush(QBrush(QColor(0, 0, 0, 120)));
}

LayerMask::~LayerMask()
{
}

void LayerMask::paint(QPainter* painter)
{
    auto full = WinFull::get();
    painter->setPen(QPen(QBrush(QColor(22, 119, 255)), maskStroke));
    painter->setBrush(QBrush(QColor(0, 0, 0, 120)));
    p.clear();
    p.addRect(full->x - 16, full->x - 16, full->w + 16, full->h + 16);
    p.addRect(maskRect);
    painter->drawPath(p);
    //p->drawImage(WinFull::get()->rect(), img);

}

void LayerMask::mousePress(QMouseEvent* event)
{
    posPress = event->pos();
    auto full = WinFull::get();
    if (full->state == State::start)
    {
        full->state = State::mask;        
        maskRect.setRect(posPress.x(), posPress.y(), 0, 0);
        event->accept();
        return;
    }
    //if (full->state == State::tool)
    //{
    //    full->toolMain->hide();
    //    if (mousePosState != 0)
    //    {
    //        changeMaskRect(dragPosition);
    //    }
    //    event->accept();
    //    return;
    //}
    //if (full->state > State::tool && mousePosState > 0) {
    //    full->toolMain->hide();
    //    full->toolSub->hide();
    //    event->accept();
    //    return;
    //}
}

void LayerMask::mouseDrag(QMouseEvent* event)
{
    auto full = WinFull::get();
    auto pos = event->pos();
    if (full->state == State::mask)
    {
        maskRect.setBottomRight(pos);
        img.fill(QColor(0, 0, 0, 0));
        p.clear();
        p.addRect(full->x - 16, full->x - 16, full->w + 16, full->h + 16);
        p.addRect(maskRect);
        full->update();
        return;
    }
}

void LayerMask::mouseRelease(QMouseEvent* event)
{
}

void LayerMask::mouseMove(QMouseEvent* event)
{
}
