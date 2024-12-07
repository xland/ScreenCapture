#pragma once

#include <QWidget>
#include <QImage>
#include <QPainter>
#include <QPaintEvent>

#include "WinBaseLayer.h"

class WinMask  : public WinBaseLayer
{
	Q_OBJECT
public:
	WinMask(QWidget* parent = nullptr);
	~WinMask();
	void initWinRects();
	void mousePress(QMouseEvent* event);
	void mouseDrag(QMouseEvent* event);
	void mouseRelease(QMouseEvent* event);
	void mouseMove(QMouseEvent* event);
public:
	QRectF maskRect;
protected:
	void paintEvent(QPaintEvent* event) override;
	void ready() override;
private:
	void changeMaskRect(const QPoint& pos);
	void changeMousePosState(const int& x, const int& y);
	void changeMousePosState2(const int& x, const int& y);
	QPoint getNativePos();
	QPoint getNativeWinPos();
private:
	QImage img;
	QPoint posPress;
	qreal maskStroke{ 2.0 };
	int mousePosState{ -1 };
	QList<QRect> winNativeRects;
	QList<HWND> winHwnds;
	int mouseInRectIndex{ -1 };
};
