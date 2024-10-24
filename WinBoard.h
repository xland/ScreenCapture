#pragma once

#include <QWidget>
#include <qimage.h>
#include <qpainter.h>
#include <QMouseEvent>
#include <QPoint>
#include <qpainterpath.h>
#include <vector>
#include <Windows.h>
#include <QGraphicsView>
#include "State.h"


class ToolMain;
class ToolSub;
class WinMask;
class WinCanvas;
class ShapeDragger;
class ShapeBase;
class WinBoard : public QWidget
{
    Q_OBJECT
public:
    WinBoard(QWidget* parent = nullptr);
    ~WinBoard();
    void dispatchEvent(QGraphicsSceneHoverEvent* e);
public:

protected:
    void paintEvent(QPaintEvent* event) override;
private:
    void initImgs();
private:
    qreal maskStroke{ 1.5 };
    bool dragging = false;
    QPoint dragPosition;
    QImage imgBg;
    QPixmap desktopImg;

};
