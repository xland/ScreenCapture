#pragma once

#include <QWidget>
#include <QTimer>

class ShapeBase;
class Canvas  : public QWidget
{
	Q_OBJECT
public:
	Canvas(QWidget *parent);
	~Canvas();
	void changeShape(ShapeBase* shape, bool forceUpdate = false);
	ShapeBase* curShape;
protected:
	void paintEvent(QPaintEvent* event) override;
private:
	void onTimeout();
private:
	QTimer* timer;
};
