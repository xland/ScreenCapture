#include "CanvasWidget.h"
#include <qlayout.h>
#include <qpushbutton.h>

CanvasWidget::CanvasWidget(QWidget *parent)
	: QWidget(parent)
{
	setWindowFlags(Qt::FramelessWindowHint);
	setStyleSheet("background-color: #A12D45;");

	QVBoxLayout* layout = new QVBoxLayout(this);
	QPushButton* button = new QPushButton("Click Me", this);
	layout->addWidget(button);

	ui.setupUi(this);
	show();
}

CanvasWidget::~CanvasWidget()
{}
