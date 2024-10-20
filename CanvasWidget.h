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


class ToolMain;
class ToolSub;
class CutMask;
class ShapeBase;
class CanvasWidget : public QWidget
{
	Q_OBJECT
public:
	CanvasWidget(QWidget *parent = nullptr);
	~CanvasWidget();
	static void Init();
	static CanvasWidget* Get();
	void changeState(const State& state);
public:
	State state{ State::start };
	ToolMain* toolMain;
	ToolSub* toolSub;
	CutMask* cutMask;
	std::vector<ShapeBase*> shapes;
protected:
	void paintEvent(QPaintEvent* event) override;
	void closeEvent(QCloseEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
private:
	void initImgs();
	void raiseTools();
private:
	qreal maskStroke{ 1.5 };
	bool dragging = false;
	QPoint dragPosition;

	std::unique_ptr<QImage> imgBg;
	QPixmap desktopImg;
	
};
