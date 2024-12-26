#pragma once

#include <QObject>
#include <QImage>
#include <QPainter>
#include <QPaintEvent>

#include "WinBase.h"

class WinMask  : public WinBase
{
	Q_OBJECT
public:
	WinMask(QObject* parent = nullptr);
	~WinMask();
	void initWinRects();
	void mousePress(QMouseEvent* event);
	void mouseDrag(QMouseEvent* event);
	void mouseRelease(QMouseEvent* event);
	void mouseMove(QMouseEvent* event);
	void update();
	void moveByKey(const int& key) override;
public:
	QRect maskRect;
protected:
private:
	void changeMaskRect(const QPoint& pos);
	void changeMousePosState(const int& x, const int& y);
	void changeMousePosState2(const int& x, const int& y);
	void paintMaskRectInfo(QPainter& p);
	void paintMaskRectBorder(QPainter& p);
	void moveMaskRect(const QPoint& pos);
private:
	QPoint posPress;
	qreal maskStroke{ 2.0 };
	int mousePosState{ -1 };
	QList<QRect> winNativeRects;
	QList<HWND> winHwnds;
};
