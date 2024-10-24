#include <qcursor.h>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsScene>

#include "WinCanvas.h"
#include "WinBoard.h"
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
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_NoSystemBackground, true);
    setAttribute(Qt::WA_QuitOnClose, false);
    setFixedSize(3840, 2160);
    show();
}

WinCanvas::~WinCanvas()
{
}

void WinCanvas::paintEvent(QPaintEvent* event)
{
    if (!shape) { return; }
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    shape->paint(&painter);
}


