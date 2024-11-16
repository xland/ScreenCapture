#pragma once

#include <QMouseEvent>
#include <QPainter>
#include <QObject>
#include <QPainterPath>

#include "ShapeBase.h"
#include "ShapeTextInput.h"

class ShapeText:public ShapeBase
{
    Q_OBJECT
public:
    ShapeText(QObject* parent = nullptr);
    ~ShapeText();
    virtual void paint(QPainter* painter) override;
    void paintDragger(QPainter* painter) override;
    void paintBorder(QPainter* painter);

    void mouseMove(QMouseEvent* event) override;
    void mouseDrag(QMouseEvent* event) override;
    void mousePress(QMouseEvent* event) override;
    void mouseRelease(QMouseEvent* event) override;
public:
    QColor color{ Qt::red };
    qreal fontSize;
    bool bold;
    bool italic;
    QString textVal;
protected:
private:
    void adjustSize();
    void createTextEdit();
    void focusOut();
private:
    QPointF startPos,endPos,pressPos;
    ShapeTextInput* textEdit;
    qreal rotate{45};
};
