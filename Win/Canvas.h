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
	void setCurShape(ShapeBase* shape);
	void removeShapeFromBoard(ShapeBase* shape);
public:
	ShapeBase* shapeCur;
private:
	void addShape();
private:
	QList<ShapeBase*> shapes;
	QTimer* timerDragger;
};
