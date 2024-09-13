#pragma once

#include <QWidget>
#include <qimage.h>
#include <qpainter.h>
#include <vector>
#include <Windows.h>

class CanvasWidget : public QWidget
{
	Q_OBJECT

public:
	CanvasWidget(const int& x, const int& y, const int& w,const int& h,QWidget *parent = nullptr);
	~CanvasWidget();
private:
	void onButtonClicked();
	void paintEvent(QPaintEvent* event) override;
	void shotScreen();
	void initPainter();
private:
	std::vector<RECT> screenRects;
	std::vector<unsigned char> bgPix;
	QImage* bg;
	QImage* board;
	QImage* canvas;
	QImage* mask;
	QPainter* boardPainter;
	QPainter* canvasPainter;
	QPainter* maskPainter;
	int x,y,w, h;
};
