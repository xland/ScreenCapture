#pragma once

#include <QWidget>

class ShapeBase  : public QWidget
{
	Q_OBJECT

public:
	ShapeBase(QWidget *parent);
	~ShapeBase();
};
