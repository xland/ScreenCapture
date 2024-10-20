#include <qpainter.h>
#include <QMouseEvent>

#include "ShapeEllipse.h"
#include "../CanvasWidget.h"
#include "../Tool/ToolSub.h"

ShapeEllipse::ShapeEllipse(const QPoint& pos,QWidget *parent) : ShapeBase(parent)
{
    setVisible(true);
	setMouseTracking(true);
	move(0, 0);
	auto size = parent->size();
	setFixedSize(size);
	setFocusPolicy(Qt::StrongFocus);
    rectShape.setTopLeft(pos);
    rectShape.setSize(QSize(0,0));
    auto toolSub = CanvasWidget::Get()->toolSub;
    isFill = toolSub->getSelectState("rectFill");
    color = toolSub->getColor();
    strokeWidth = toolSub->getStrokeWidth();
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

void ShapeEllipse::PressEvent(QMouseEvent* event)
{

}

void ShapeEllipse::MoveEvent(QMouseEvent* event)
{
    if(state == ShapeState::ready) return;
    auto pos = event->pos();
    if (event->buttons() & Qt::LeftButton) {
        state = ShapeState::sizing;
        rectShape.setBottomRight(pos);
        update();
    }
}

void ShapeEllipse::ReleaseEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton) {
        state = ShapeState::ready;
    }
}
