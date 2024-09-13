#pragma once

#include <QWidget>
#include <qimage.h>
#include <qpainter.h>
#include <QMouseEvent>
#include <QPoint>
#include <qpainterpath.h>
#include <vector>
#include <Windows.h>
#include "State.h"

class CanvasWidget : public QWidget
{
	Q_OBJECT
public:
	CanvasWidget(const int& x, const int& y, const int& w,const int& h,QWidget *parent = nullptr);
	~CanvasWidget();
protected:
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
private:
	void onButtonClicked();
	void paintEvent(QPaintEvent* event) override;
	void shotScreen();
	void initPainter();
	void paintMask(QPainter& painter);
private:
	int x, y, w, h;
	State state{State::start};
	QRect maskRect;
	qreal maskStroke{ 1.5 };
	bool dragging = false;
	QPoint dragPosition;

	std::unique_ptr<QImage> imgBg;
	std::unique_ptr<QImage> imgBoard;
	std::unique_ptr<QImage> imgCanvas;
	
};
