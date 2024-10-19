#include <qpainter.h>

#include "ShapeRect.h"

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


}

void ShapeRect::mousePressEvent(QMouseEvent* event)
{
}

void ShapeRect::mouseMoveEvent(QMouseEvent* event)
{
}

void ShapeRect::mouseReleaseEvent(QMouseEvent* event)
{
}
