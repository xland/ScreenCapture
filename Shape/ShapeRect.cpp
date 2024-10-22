#include <qpainter.h>
#include <QMouseEvent>

#include "ShapeRect.h"
#include "../CanvasWidget.h"
#include "../Tool/ToolSub.h"

ShapeRect::ShapeRect(QWidget* parent) : ShapeBase(parent)
{
    setVisible(true);
    move(0, 0);
    auto size = parent->size();
    setFixedSize(size);
    rectShape.setSize(QSize(0, 0));
}

ShapeRect::~ShapeRect()
{
}

void ShapeRect::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    if (isFill)
    {
        painter.setPen(Qt::NoPen);
        painter.setBrush(color);
    }
    else
    {
        QPen pen(color, strokeWidth);
        painter.setPen(pen);
    }
    painter.drawRect(rectShape);
}

void ShapeRect::mousePressEvent(QMouseEvent* event)
{
    if (state == ShapeState::ready)
    {
        return;
    }
    rectShape.setTopLeft(event->pos());
    auto toolSub = CanvasWidget::Get()->toolSub;
    isFill = toolSub->getSelectState("rectFill");
    color = toolSub->getColor();
    strokeWidth = toolSub->getStrokeWidth();
}

void ShapeRect::mouseMoveEvent(QMouseEvent* event)
{
    auto pos = event->pos();
    if (state == ShapeState::ready)
    {
        // qDebug() << "ready!!!!";
        // auto innerRect = rectShape.adjusted(strokeWidth / 2, strokeWidth / 2, 0 - strokeWidth / 2, 0 - strokeWidth / 2);
        // auto outerRect = rectShape.adjusted(0 - strokeWidth / 2, 0 - strokeWidth / 2, strokeWidth / 2, strokeWidth / 2);
        // if (outerRect.contains(pos) && !innerRect.contains(pos))
        // {
        //     setCursor(Qt::SizeAllCursor);
        // }
        // else
        // {
        //     unsetCursor();
        // }
        return;
    }
    if (event->buttons() & Qt::LeftButton)
    {
        state = ShapeState::sizing;
        rectShape.setBottomRight(pos);
        update();
    }
}

void ShapeRect::mouseReleaseEvent(QMouseEvent* event)
{
    state = ShapeState::ready;
    auto canvasWidget = CanvasWidget::Get();
    canvasWidget->addShape();
}
