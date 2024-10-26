#pragma once

#include <QWidget>
#include <QMouseEvent>
#include <QGraphicsItem>
#include <QGraphicsPathItem>
#include <dwmapi.h>
#include <qpen.h>
#include <qbrush.h>
#include "State.h"

class WinMask  : public QWidget
{
    Q_OBJECT
public:
    WinMask(QWidget* parent = nullptr);
    ~WinMask();
    void mousePress(QMouseEvent* event);
    void mouseDrag(QMouseEvent* event);
    void mouseRelease(QMouseEvent* event);
    void mouseMove(QMouseEvent* event);
public:
    QRect maskRect;
    bool isShapeUseMouse{ false };

protected:
    void paintEvent(QPaintEvent* event) override;
    void showEvent(QShowEvent* event) override;

private:
    void changeMaskRect(const QPoint& pos);
    void changeMousePosState(const int& x,const int& y);
    void changeMousePosState2(const int& x, const int& y);
private:
    QPainterPath p;
    qreal maskStroke{ 1.8 };
    QPoint dragPosition;
    int mousePosState{ -1 };
    std::vector<QRect> winRects;
};
