#include <qlayout.h>
#include <qpushbutton.h>
#include <QEvent>

#include "App.h"
#include "WinBoard.h"
#include "WinCanvas.h"
#include "WinMask.h"
#include "WinFull.h"
#include "Tool/ToolMain.h"
#include "Tool/ToolSub.h"
#include "Shape/ShapeDragger.h"
#include "Shape/ShapeBase.h"
#include "Shape/ShapeRect.h"
#include "Shape/ShapeEllipse.h"

WinBoard::WinBoard(QWidget* parent) : QWidget(parent)
{
    setMouseTracking(true);
    setCursor(Qt::CrossCursor);
    //initImgs();  
}

WinBoard::~WinBoard()
{
}

void WinBoard::addShape(const QPoint& pos)
{
    for (auto it = shapes.begin(); it != shapes.end(); ) {
        if ((*it)->state == ShapeState::temp) {
            it = shapes.erase(it);
        }
        else {
            ++it;
        }
    }
    ShapeBase* shape;
    auto full = App::getFull();
    if (full->state == State::rect) {
        shape = new ShapeRect(this);
    }
    else if (full->state == State::ellipse) {
        shape = new ShapeEllipse(this);
    }
    else
    {
        return;
    }
    connect(shape, &ShapeBase::onHover, App::getFullCanvas(), &WinCanvas::onShapeHover);
    shapes.push_back(shape);
}

void WinBoard::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setRenderHint(QPainter::LosslessImageRendering, true);
    // painter.drawImage(rect(), imgBg);
    painter.drawPixmap(rect(), App::getFull()->bgImg);
    for (size_t i = 0; i < shapes.size(); i++)
    {
        if (shapes[i]->state != ShapeState::ready) {
            continue;
        }
        shapes[i]->paint(&painter);
    }
}

void WinBoard::mousePressEvent(QMouseEvent* event)
{
    //lower(); //不如native event fillter
    event->ignore();
    emit mousePress(event);
    if (!event->isAccepted()) {
        addShape(event->pos());
        emit mousePress(event); //不然新添加的Shape收不到鼠标按下事件
    }
}

void WinBoard::mouseMoveEvent(QMouseEvent* event)
{
    event->ignore();
    if (event->buttons() == Qt::NoButton) {
        emit mouseMove(event);
    }
    else {
        emit mouseDrag(event);
    }
}

void WinBoard::mouseReleaseEvent(QMouseEvent* event)
{
    event->ignore();
    emit mouseRelease(event);
}

void WinBoard::showEvent(QShowEvent* event)
{
    hwnd = (HWND)winId();
    SetWindowPos(hwnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOSENDCHANGING);
}


