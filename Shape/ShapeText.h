#pragma once

#include <QMouseEvent>
#include <QPainter>
#include <QObject>
#include <QPainterPath>
#include <QTextEdit>
#include "ShapeBase.h"

class ShapeText:public ShapeBase
{
    Q_OBJECT
public:
    ShapeText(QObject* parent = nullptr);
    ~ShapeText();
    virtual void paint(QPainter* painter) override;
    void paintDragger(QPainter* painter) override;
    void mouseMove(QMouseEvent* event) override;
    void mouseDrag(QMouseEvent* event) override;
    void mousePress(QMouseEvent* event) override;
    void mouseRelease(QMouseEvent* event) override;
public:
    QColor color{ Qt::red };
    qreal strokeWidth;
protected:
private:
    void resetDragger();
private:
    QPointF startPos,endPos,pressPos;
    QTextEdit* textEdit;
    qreal rotate{45};
};
