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
	Canvas(const QImage& img,QObject *parent);
	~Canvas();
	void undo();
	void redo();
	void mousePress(QMouseEvent* event);
	void mouseDrag(QMouseEvent* event);
	void mouseRelease(QMouseEvent* event);
	void mouseMove(QMouseEvent* event);
	void paint(QPainter& p);
	void setHoverShape(ShapeBase* shape);
	void removeShapeFromBoard(ShapeBase* shape);
	void paintShapeOnBoard(ShapeBase* shape);
	void removeShapeCur();
	void copyColor(const int& key);
public:
	ShapeBase* shapeCur;
	ShapeBase* shapeHover;
	QList<ShapeBase*> shapes;
	QImage imgBg;
	QImage imgBoard;
	QImage imgCanvas;
private:
	void addShape();
private:
	QTimer* timerDragger;
};
