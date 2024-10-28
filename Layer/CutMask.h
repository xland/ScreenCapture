#pragma once

#include <QObject>
#include <QPainter>
#include <dwmapi.h>
#include <qpoint.h>
#include <QPaintEvent>
#include <QPainterPath>
#include <QMouseEvent>
#include <QWidget>

class CutMask  : public QWidget
{
	Q_OBJECT
public:
	CutMask(QWidget* parent = nullptr);
	~CutMask();
	void mousePress(QMouseEvent* event);
	void mouseDrag(QMouseEvent* event);
	void mouseRelease(QMouseEvent* event);
	void mouseMove(QMouseEvent* event);
public:
	QRect maskRect;
protected:
	void paintEvent(QPaintEvent* event) override;
	void showEvent(QShowEvent* event) override;
private:
	void changeMaskRect(const QPoint& pos);
	void changeMousePosState(const int& x, const int& y);
	void changeMousePosState2(const int& x, const int& y);
private:
	QPainterPath p;
	QPoint posPress;
	qreal maskStroke{ 1.8 };
	int mousePosState{ -1 };
};
