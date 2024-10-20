#include <qpainter.h>
#include <QMouseEvent>

#include "ShapeRect.h"
#include "../CanvasWidget.h"
#include "../Tool/ToolSub.h"

ShapeRect::ShapeRect(QWidget *parent) : ShapeBase(parent)
{
	setMouseTracking(true);
	move(0, 0);
	auto size = parent->size();
	setFixedSize(size);
	setFocusPolicy(Qt::StrongFocus);
}

ShapeRect::~ShapeRect()
{
}

void ShapeRect::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);
    if(isFill){
        painter.setPen(Qt::NoPen);
        painter.setBrush(color);
    }else{
        QPen pen(color, strokeWidth);
        painter.setPen(pen);
    }
    painter.drawRect(rectShape);
}

void ShapeRect::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        rectShape.setTopLeft(event->pos());
        rectShape.setSize(QSize(0,0));
        auto toolSub = CanvasWidget::Get()->toolSub;
        isFill = toolSub->getSelectState("rectFill");
        color = toolSub->getColor();
        strokeWidth = toolSub->getStrokeWidth();
    }
}

void ShapeRect::mouseMoveEvent(QMouseEvent* event)
{
    auto pos = event->pos();
    if (event->buttons() & Qt::LeftButton) {
        if(isTemp) isTemp = false;
        rectShape.setBottomRight(pos);
        update();
    }
}

void ShapeRect::mouseReleaseEvent(QMouseEvent* event)
{
    auto canvasWidget = CanvasWidget::Get();
    canvasWidget->addShape();
}
