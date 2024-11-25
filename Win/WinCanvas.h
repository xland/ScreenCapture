#pragma once

#include <QWidget>
#include <QTimer>

class ShapeBase;
class WinBase;
class WinCanvas : public QWidget
{
	Q_OBJECT
public:
	WinCanvas(QWidget *parent = nullptr);
	~WinCanvas();
	void refresh(ShapeBase* shape, bool forceUpdate = false);
	void init(WinBase* winBase);
public:
	ShapeBase* curShape;
	WinBase* winBase;
protected:
	void paintEvent(QPaintEvent* event) override;
private:
	void onTimeout();
	void initTimer();
	void initWindow();
private:
	QTimer* timer;
};
