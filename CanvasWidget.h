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
class CutMask;
class ShapeRect;
class CanvasWidget : public QWidget
{
    Q_OBJECT
public:
    CanvasWidget(QWidget* parent = nullptr);
    ~CanvasWidget();
    static void Init();
    static CanvasWidget* Get();
    void dispatchEvent(QGraphicsSceneHoverEvent* e);
    void addShape();
public:
    State state{ State::start };
    ToolMain* toolMain;
    ToolSub* toolSub;
    CutMask* cutMask;
    std::vector<QGraphicsItem*> shapes;
    QGraphicsScene* scene;
    QGraphicsView* view;
protected:
    void closeEvent(QCloseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
private:
    void initImgs();
private:
    qreal maskStroke{ 1.5 };
    bool dragging = false;
    QPoint dragPosition;
    ShapeRect* shapeRect;
    QImage imgBg;
    QPixmap desktopImg;

};
