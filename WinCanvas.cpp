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
    setAttribute(Qt::WA_TransparentForMouseEvents, true);
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


