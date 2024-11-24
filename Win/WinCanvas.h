#pragma once

#include <QWidget>
#include <QTimer>

class ShapeBase;
class WinCanvas : public QWidget
{
	Q_OBJECT
public:
	WinCanvas(QWidget *parent);
	~WinCanvas();
	void changeShape(ShapeBase* shape, bool forceUpdate = false);
	ShapeBase* curShape;
protected:
	void paintEvent(QPaintEvent* event) override;
private:
	void onTimeout();
private:
	QTimer* timer;
};
