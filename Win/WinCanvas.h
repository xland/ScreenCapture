#pragma once

#include <QObject>
#include <QTimer>

#include "WinBase.h"

class ShapeBase;
class WinCanvas : public WinBase
{
	Q_OBJECT
public:
	WinCanvas(QWidget* parent = nullptr);
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
