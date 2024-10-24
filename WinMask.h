#pragma once

#include <QWidget>
#include <QMouseEvent>
#include <QGraphicsItem>
#include <QGraphicsPathItem>
#include <qpen.h>
#include <qbrush.h>
#include "State.h"

class WinMask  : public QWidget
{
    Q_OBJECT
public:
    WinMask(QWidget* parent = nullptr);
    ~WinMask();
public:
    QRect maskRect;
    bool isShapeUseMouse{ false };
signals:
    void mouseMove(QMouseEvent* e);
    void mouseDrag(QMouseEvent* e);
    void mousePress(QMouseEvent* e);
    void mouseRelease(QMouseEvent* e);
protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    void changeMaskRect(const QPoint& pos);
    void changeMousePosState(const int& x,const int& y);
    void changeMousePosState2(const int& x, const int& y);
private:
    QPainterPath p;
    qreal maskStroke{ 1.8 };
    QPoint dragPosition;
    int mousePosState{ -1 };
};
