#pragma once

#include <QWidget>
#include "ShapeBase.h"

class ShapeRect: public ShapeBase
{
	Q_OBJECT

public:
	ShapeRect(QWidget *parent);
	~ShapeRect();
public:
	bool isFill{ false };
	qreal strokeWidth{ 2.0f };
	QColor color{ Qt::red };
protected:
	void paintEvent(QPaintEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
};
