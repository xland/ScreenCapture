#pragma once

#include <QObject>
#include <qpoint.h>
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include "LayerBase.h"

class LayerMask  : public LayerBase
{
	Q_OBJECT
public:
	LayerMask(QObject *parent);
	~LayerMask();
	void paint(QPainter* painter) override;
	void mousePress(QMouseEvent* event);
	void mouseDrag(QMouseEvent* event);
	void mouseRelease(QMouseEvent* event);
	void mouseMove(QMouseEvent* event);
public:
	QRectF maskRect;
private:
	QPainter painter;
	QPainterPath p;
	QPointF posPress;
	qreal maskStroke{ 1.8 };
};
