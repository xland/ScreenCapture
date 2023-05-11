
#ifndef CANVAS_H
#define CANVAS_H


#include <QObject>
#include <QWidget>
#include <QPainter>
#include <QPainterPath>



class Canvas : public QWidget
{
    Q_OBJECT
public:
    explicit Canvas(QWidget* parent = nullptr);
    ~Canvas();
    enum class State
    {
        start,
        rect,
        rectFill,
        ellipse,
        ellipseFill
    };
    void paintEvent(QPaintEvent*) override;
public:
    State state = State::start;
protected:
    void mouseMoveEvent(QMouseEvent* mouseEvent) override;
    void mousePressEvent(QMouseEvent* mouseEvent) override;
    void mouseReleaseEvent(QMouseEvent* mouseEvent) override;
signals:
private:
    void initMask();
private:
    QVector<QPainterPath> paths;
    QPointF mousePressPoint;
    bool isMouseDown = false;
    QPainterPath maskPath;
    qreal maskBorderWidth = 2.0;
    QPainter* painter;
    QImage* canvasImg;
};

#endif // CANVAS_H
