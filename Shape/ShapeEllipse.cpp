#include <qpainter.h>
#include <QMouseEvent>

#include "ShapeEllipse.h"
#include "../CanvasWidget.h"
#include "../Tool/ToolSub.h"

ShapeEllipse::ShapeEllipse(QWidget *parent) : ShapeBase(parent)
{
    setVisible(true);
	setMouseTracking(true);
	move(0, 0);
	auto size = parent->size();
	setFixedSize(size);
	setFocusPolicy(Qt::StrongFocus);
}

ShapeEllipse::~ShapeEllipse()
{
}

void ShapeEllipse::paintEvent(QPaintEvent* event)
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
    painter.drawEllipse(rectShape);
}

void ShapeEllipse::mousePressEvent(QMouseEvent* event)
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

void ShapeEllipse::mouseMoveEvent(QMouseEvent* event)
{
    auto pos = event->pos();
    if (event->buttons() & Qt::LeftButton) {
        if(isTemp) isTemp = false;
        rectShape.setBottomRight(pos);
        update();
    }
}

void ShapeEllipse::mouseReleaseEvent(QMouseEvent* event)
{
    auto canvasWidget = CanvasWidget::Get();
    canvasWidget->addShape();
}
