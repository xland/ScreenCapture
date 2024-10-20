#include <qpainter.h>
#include <QMouseEvent>

#include "ShapeRect.h"
#include "../CanvasWidget.h"
#include "../Tool/ToolSub.h"

ShapeRect::ShapeRect(const QPoint& pos,QWidget *parent) : ShapeBase(parent)
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

void ShapeRect::PressEvent(QMouseEvent* event)
{

}

void ShapeRect::MoveEvent(QMouseEvent* event)
{
    if(state == ShapeState::ready){
        return;
    }
    auto pos = event->pos();
    if (event->buttons() & Qt::LeftButton) {
        state = ShapeState::sizing;
        rectShape.setBottomRight(pos);
        update();
    }
}

void ShapeRect::ReleaseEvent(QMouseEvent* event)
{
    state = ShapeState::ready;
}
