#pragma once

#include <QWidget>
#include <QMenu>
#include <QAction>
#include "WinBase.h"

class WinPin  : public WinBase
{
	Q_OBJECT

public:
	WinPin(const QRect& r, const QImage& img,QWidget *parent = nullptr);
	~WinPin();
protected:
	void paintEvent(QPaintEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
private:
private:
	QPointF posPress;
};
