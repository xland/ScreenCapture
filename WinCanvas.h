#pragma once

#include <QWidget>
#include <QMouseEvent>
#include <QGraphicsItem>
#include <QGraphicsPathItem>
#include <qpen.h>
#include <qbrush.h>
#include <qtimer.h>
#include "State.h"

class ShapeBase;
class WinCanvas  : public QWidget
{
    Q_OBJECT
public:
    WinCanvas(QWidget* parent = nullptr);
    ~WinCanvas();
    void changeShape(ShapeBase* shape,bool forceUpdate=false);
    ShapeBase* curShape;
public:

protected:
    void paintEvent(QPaintEvent* event) override;
private:
    void onTimeout();
private:
    QTimer* timer;
};
