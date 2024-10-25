#pragma once

#include <QWidget>
#include <QGraphicsItem>
#include "ShapeState.h"

class ShapeBase:public QObject
{
    Q_OBJECT
public:
    ShapeBase(const QPoint& pos,QObject* parent = nullptr);
    virtual ~ShapeBase();
    virtual void paint(QPainter* painter) = 0;
signals:
    void onActived(ShapeBase* e);
public:
    int draggerSize{ 8 };
    ShapeState state{ ShapeState::sizing4 };
    QPoint startPos;
    int hoverDraggerIndex{ -1 };
    std::vector<QRect> draggers;
};
