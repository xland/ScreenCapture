#pragma once

#include <QWidget>
#include <QTimer>

#include "WinBaseLayer.h"

class ShapeBase;
class WinBaseLayer;
class WinCanvas : public WinBaseLayer
{
	Q_OBJECT
public:
	WinCanvas(QWidget *parent = nullptr);
	~WinCanvas();
	void refresh(ShapeBase* shape, bool forceUpdate = false);
public:
	ShapeBase* curShape;
protected:
	void paintEvent(QPaintEvent* event) override;
private:
	void onTimeout();
	void initTimer();
private:
	QTimer* timer;
};
