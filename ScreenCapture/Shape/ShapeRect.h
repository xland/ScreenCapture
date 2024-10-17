#pragma once

#include <QWidget>
#include "ShapeBase.h"

class ShapeRect: public ShapeBase
{
	Q_OBJECT

public:
	ShapeRect(QWidget *parent);
	~ShapeRect();
protected:
	void paintEvent(QPaintEvent* event) override;
};
