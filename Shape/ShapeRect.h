#pragma once

#include <QWidget>
#include "ShapeBase.h"

class ShapeRect: public ShapeBase
{
    Q_OBJECT

public:
    ShapeRect(QWidget* parent);
    ~ShapeRect();
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
