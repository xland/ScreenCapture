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
	QRect maskRect;
protected:
	void paintEvent(QPaintEvent* event) override;
private:
	void changeMaskRect(const QPoint& pos);
	void changeMousePosState(const int& x, const int& y);
	void changeMousePosState2(const int& x, const int& y);
	void initWinRects();
	void initWindow();
private:
	QPainterPath p;
	QPoint posPress;
	qreal maskStroke{ 1.8 };
	int mousePosState{ -1 };
	std::vector<QRect> winRects;
};
