#pragma once

#include <QMouseEvent>
#include <QPainter>
#include <QObject>
#include <QWindow>

#include "ShapeBase.h"
#include "ShapeTextInput.h"

class ShapeText:public ShapeBase
{
    Q_OBJECT
public:
    ShapeText(QObject* parent = nullptr);
    ~ShapeText();
    //void focusOut(QWindow* focusWindow);
    void focusOut();
    void focusIn();
public:
    QColor color{ Qt::red };
    qreal fontSize;
    bool bold;
    bool italic;
    QString textVal;
protected:
    virtual void paint(QPainter* painter) override;
    void paintDragger(QPainter* painter) override;
    void mouseMove(QMouseEvent* event) override;
    void mouseDrag(QMouseEvent* event) override;
    void mousePress(QMouseEvent* event) override;
    void mouseRelease(QMouseEvent* event) override;
private:
private:
    QPointF pressPos;
    ShapeTextInput* textEdit;
    qreal rotate{45};
};
