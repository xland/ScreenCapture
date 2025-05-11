#pragma once

#include <QMouseEvent>
#include <QPainter>
#include <QObject>
#include <QPainterPath>
#include "ShapeBase.h"

class ShapeNumber:public ShapeBase
{
    Q_OBJECT
public:
    ShapeNumber(QObject* parent = nullptr);
    ~ShapeNumber();
    static void resetValBy(const int& num);
    virtual void paint(QPainter* painter) override;
    void paintDragger(QPainter* painter) override;
    bool mouseMove(QMouseEvent* event) override;
    void mouseDrag(QMouseEvent* event) override;
    bool mousePress(QMouseEvent* event) override;
    bool mouseRelease(QMouseEvent* event) override;
public:
    QPainterPath shape;
    bool isFill{ false };
    QColor color{ Qt::red };
protected:
private:
    void resetShape();
	void resetDraggers();
private:
    QPointF centerPos,pressPos,arrowPos,sizePos;
    unsigned int val;
    qreal r{26.f}, angle{ 270.0 };
};
