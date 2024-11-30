#pragma once

#include <QObject>
#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QPainterPath>
#include <QMouseEvent>


class WinMask  : public QWidget
{
	Q_OBJECT
public:
	WinMask(QWidget* parent = nullptr);
	~WinMask();
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
	void initWinRects();
	void initWindow();
	//void initMaxScreenDpr();
private:
	QPoint posPress;
	qreal maskStroke{ 2.0 };
	int mousePosState{ -1 };
	QList<QRectF> winRects;
	QList<QRect> winNativeRects;
	qreal maxScreenDpr;
	int mouseInRectIndex{ -1 };
};
