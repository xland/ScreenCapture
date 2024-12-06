#pragma once

#include <QWidget>
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
private:
	void changeMaskRect(const QPoint& pos);
	void changeMousePosState(const int& x, const int& y);
	void changeMousePosState2(const int& x, const int& y);
private:
	QPoint posPress;
	qreal maskStroke{ 2.0 };
	int mousePosState{ -1 };
	QList<QRect> winNativeRects;
	int mouseInRectIndex{ -1 };
};
