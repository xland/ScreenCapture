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
	void paintDragger();
	void paintShape();
	void clear();
public:
	ShapeBase* curShape;
protected:

private:
	std::shared_ptr<QPainter> getPainter();
private:
};
