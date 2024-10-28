#pragma once

#include <QWidget>

class Canvas  : public QWidget
{
	Q_OBJECT
public:
	Canvas(QWidget *parent);
	~Canvas();
protected:
	void paintEvent(QPaintEvent* event) override;
};
