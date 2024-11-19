#pragma once

#include <QObject>
#include <QPainter>
#include <dwmapi.h>
#include <qpoint.h>
#include <QPaintEvent>
#include <QPainterPath>
#include <QMouseEvent>
#include <QWidget>


class CutMask  : public QObject
{
	Q_OBJECT
public:
	CutMask(QWidget* parent = nullptr);
	~CutMask();
	void mousePress(QMouseEvent* event);
	void mouseDrag(QMouseEvent* event);
	void mouseRelease(QMouseEvent* event);
	void mouseMove(QMouseEvent* event);
	void paint(QPainter* painter);
public:
	QRect maskRect;
protected:
private:
	void changeMaskRect(const QPoint& pos);
	void changeMousePosState(const int& x, const int& y);
	void changeMousePosState2(const int& x, const int& y);
	void initWinRects();
private:
	QPainterPath p;
	QPoint posPress;
	qreal maskStroke{ 1.8 };
	int mousePosState{ -1 };
	std::vector<QRect> winRects;
};
