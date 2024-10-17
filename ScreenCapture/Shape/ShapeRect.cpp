#include <qpainter.h>

#include "ShapeRect.h"

ShapeRect::ShapeRect(QWidget *parent) : ShapeBase(parent)
{
	setMouseTracking(true);
	setGeometry(parent->rect());
}

ShapeRect::~ShapeRect()
{
}

void ShapeRect::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);
	auto g = geometry();
	painter.drawLine(g.topLeft(), g.bottomRight());
}
