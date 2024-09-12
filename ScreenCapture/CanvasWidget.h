#pragma once

#include <QWidget>
#include "ui_CanvasWidget.h"

class CanvasWidget : public QWidget
{
	Q_OBJECT

public:
	CanvasWidget(QWidget *parent = nullptr);
	~CanvasWidget();

private:
	Ui::CanvasWidgetClass ui;
};
