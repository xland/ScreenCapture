#include <qcursor.h>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsScene>

#include "WinCanvas.h"
#include "WinBoard.h"
#include "App.h"
#include "Tool/ToolMain.h"
#include "Tool/ToolSub.h"
#include "WinFull.h"
#include "Shape/ShapeBase.h"
#include "Shape/ShapeDragger.h"
#include "Shape/ShapeRect.h"
#include "Shape/ShapeEllipse.h"
#include "WinCanvas.h"

namespace {
    WinCanvas* canvas;
}

WinCanvas::WinCanvas(QWidget* parent) : QWidget(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents, true);
    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, this, &WinCanvas::onTimeout);
}

WinCanvas::~WinCanvas()
{
}

void WinCanvas::changeShape(ShapeBase* shape, bool forceUpdate)
{
    if (shape) {
        timer->start(800);
    }
    if (shape != curShape) {
        curShape = shape;
        if (shape) {
            update();
        }
    }
    if (forceUpdate) {
        update();
    }
}

void WinCanvas::paintEvent(QPaintEvent* event)
{
    if (!curShape) return;
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    if (curShape->state >= ShapeState::sizing0) {
        curShape->paint(&painter);
    }
    else {
        curShape->paintDragger(&painter);
    }
}

void WinCanvas::onTimeout()
{
    update();
}


