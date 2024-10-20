#pragma once

#include <QWidget>
#include "ShapeBase.h"

class ShapeEllipse: public ShapeBase
{
	Q_OBJECT

public:
    ShapeEllipse(QWidget *parent);
    ~ShapeEllipse();
public:
protected:
	void paintEvent(QPaintEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
private:
    QPoint posPress;
    QRect rectShape;
    bool isFill{ false };
    int strokeWidth{ 2 };
    QColor color{ Qt::red };
};
