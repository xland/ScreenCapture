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
#include "ToolMain.h"

class CanvasWidget : public QWidget
{
	Q_OBJECT
public:
	CanvasWidget(QWidget *parent = nullptr);
	~CanvasWidget();
	static void Init();
	static CanvasWidget* Get();
public:
	QRect maskRect;
protected:
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void paintEvent(QPaintEvent* event) override;
private:
	void onButtonClicked();	
	void initImgs();
	void paintMask(QPainter& painter);
private:
	State state{State::start};
	qreal maskStroke{ 1.5 };
	bool dragging = false;
	QPoint dragPosition;
	ToolMain* toolMain;

	std::unique_ptr<QImage> imgBg;
	
};
