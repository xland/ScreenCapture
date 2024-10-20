#pragma once

#include <QWidget>
#include "ShapeBase.h"

class ShapeEllipse: public ShapeBase
{
	Q_OBJECT

public:
    ShapeEllipse(const QPoint& pos,QWidget *parent);
    ~ShapeEllipse();
    void PressEvent(QMouseEvent* event) override;
    void MoveEvent(QMouseEvent* event) override;
    void ReleaseEvent(QMouseEvent* event) override;
public:
protected:
    void paintEvent(QPaintEvent* event) override;
private:
    QPoint posPress;
    QRect rectShape;
    bool isFill{ false };
    int strokeWidth{ 2 };
    QColor color{ Qt::red };
};
