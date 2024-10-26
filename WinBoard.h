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
    ShapeBase* addShape();
public:
    std::vector<ShapeBase*> shapes;
    HWND hwnd{nullptr};
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
    void showEvent(QShowEvent* event) override;
private:
private:
    qreal maskStroke{ 1.5 };
    bool dragging = false;
    QPoint dragPosition;
};
