
#ifndef CANVAS_H
#define CANVAS_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsPathItem>
#include <QObject>


class Canvas : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit Canvas(QObject* parent = nullptr);
    ~Canvas();
protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent) override;
private:
    void initMask();
private:
    QPointF mousePressPoint;
    bool isMouseDown = false;
    QGraphicsPathItem* maskPathItem;
    qreal maskBorderWidth = 3.0;
};

#endif // CANVAS_H
