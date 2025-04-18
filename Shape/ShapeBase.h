#pragma once

#include <QObject>
#include <QMouseEvent>
#include <QPainter>
#include "ShapeState.h"

class ShapeBase:public QObject
{
    Q_OBJECT
public:
    ShapeBase(QObject* parent = nullptr);
    virtual ~ShapeBase();
    virtual void paint(QPainter* painter) = 0;
    virtual void paintDragger(QPainter* painter) {};
    virtual bool mouseMove(QMouseEvent* event) { return false; };
    virtual void mouseDrag(QMouseEvent* event) {};
    virtual bool mousePress(QMouseEvent* event) { return false; };
    virtual void mouseRelease(QMouseEvent* event) {};
    virtual void paintOnBoard();
    void showDragger();
public:
    int draggerSize{ 8 };
    ShapeState state{ ShapeState::temp };
    int hoverDraggerIndex{ -1 };
    std::vector<QRect> draggers;
    bool isEraser{ false };
protected:
    virtual void painting();
    virtual void paintingStart();
    virtual void paintingPrepare();
    void prepareDraggers(const int& size);

};
