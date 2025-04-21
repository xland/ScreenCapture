#pragma once

#include <QObject>
#include <QPainter>
#include <QPaintEvent>
#include <QTimer>

class ShapeBase;
class Canvas  : public QObject
{
	Q_OBJECT
public:
	Canvas(QObject *parent);
	~Canvas();
	void mousePress(QMouseEvent* event);
	void mouseDrag(QMouseEvent* event);
	void mouseRelease(QMouseEvent* event);
	void mouseMove(QMouseEvent* event);
	void paint(QPainter& p);
	void setHoverShape(ShapeBase* shape);
	void removeShapeFromBoard(ShapeBase* shape);
	void paintShapeOnBoard(ShapeBase* shape);
public:
	ShapeBase* shapeCur;
	ShapeBase* shapeHover;
private:
	void addShape();
private:
	QList<ShapeBase*> shapes;
	QTimer* timerDragger;
};
