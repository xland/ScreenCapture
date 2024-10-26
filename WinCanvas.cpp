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

void WinCanvas::onShapeHover(ShapeBase* shape)
{
    update();
    timer->start(800);
}

void WinCanvas::paintEvent(QPaintEvent* event)
{
    auto board = App::getFullBoard();
    if (!board) return;
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    for (size_t i = 0; i < board->shapes.size(); i++)
    {
        if (board->shapes[i]->state >= ShapeState::sizing0) {
            board->shapes[i]->paint(&painter);
            break;
        }
        if (board->shapes[i]->hoverDraggerIndex >=0) {
            board->shapes[i]->paintDragger(&painter);
            break;
        }
    }
}

void WinCanvas::onTimeout()
{
    update();
}


