#include <QEvent>

#include "WinBase.h"
#include "../Shape/ShapeBase.h"
#include "../Shape/ShapeRect.h"
#include "../Shape/ShapeEllipse.h"
#include "../Shape/ShapeArrow.h"
#include "../Shape/ShapeNumber.h"
#include "../Shape/ShapeLine.h"
#include "../Shape/ShapeText.h"
#include "../Shape/ShapeEraser.h"


WinBase::WinBase(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);  //必须设置，不然Board无法执行擦除效果
    setAutoFillBackground(true);
    setMouseTracking(true);
    setAttribute(Qt::WA_QuitOnClose, false);
    setWindowFlags(Qt::FramelessWindowHint); //  | Qt::WindowStaysOnTopHint
    setCursor(Qt::CrossCursor);
    //this->installEventFilter(this);
}

WinBase::~WinBase()
{
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
void WinBase::updateCursor(Qt::CursorShape cur)
{
    if (cursor().shape() != cur) {
        setCursor(cur);
    }
}

//bool WinBase::eventFilter(QObject* watched, QEvent* event)
//{
//    auto type = event->type();
//    event->ignore();
//    if (type == QEvent::MouseButtonPress) {
//        emit mousePress(static_cast<QMouseEvent*>(event));
//        return true;
//    }
//    else if (type == QEvent::MouseMove) {
//        emit mouseMove(static_cast<QMouseEvent*>(event));
//        return true;
//    }
//    else if (type == QEvent::MouseButtonRelease) {
//        emit mouseRelease(static_cast<QMouseEvent*>(event));
//        return true;
//    }
//    return QObject::eventFilter(watched, event);
//}
