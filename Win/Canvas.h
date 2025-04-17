#pragma once

#include <QObject>
#include <QPainter>
#include <QPaintEvent>

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
private:
	ShapeBase* addShape();
private:
	QList<ShapeBase*> shapes;
};
