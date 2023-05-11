
#ifndef CANVASBAK_H
#define CANVASBAK_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsPathItem>
#include <QObject>


class CanvasBak : public QGraphicsScene
{
    Q_OBJECT
public:
    enum class State
    {
        start,
        rect,
        rectFill,
        ellipse,
        ellipseFill
    };
    explicit CanvasBak(QObject* parent = nullptr);
    ~CanvasBak();
public:
    State state = State::start;
protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent) override;
private:
    void initMask();
    void initPath();
private:
    QPointF mousePressPoint;
    bool isMouseDown = false;
    QGraphicsPathItem* maskPathItem;
    QGraphicsPathItem* rectPathItem;
    qreal maskBorderWidth = 3.0;
    bool test = true;
};

#endif // CANVASBAK_H
