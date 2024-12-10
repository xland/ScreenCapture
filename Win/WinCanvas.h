#pragma once

#include <QObject>
#include <QTimer>

#include "WinBase.h"

class ShapeBase;
class WinCanvas : public WinBase
{
	Q_OBJECT
public:
	WinCanvas(QObject* parent = nullptr);
	~WinCanvas();
	void refresh(ShapeBase* shape, bool forceUpdate = false);
public:
	ShapeBase* curShape;
protected:

private:
	void onTimeout();
	void initTimer();
	void update();
private:
	QTimer* timer;
};
