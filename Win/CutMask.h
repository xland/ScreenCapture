#pragma once

#include <QObject>
#include <QImage>
#include <QPainter>
#include <QPaintEvent>

class CutMask  : public QObject
{
	Q_OBJECT
public:
	CutMask(QObject* parent = nullptr);
	~CutMask();
	void mousePress(QMouseEvent* event);
	void mouseDrag(QMouseEvent* event);
	void mouseRelease(QMouseEvent* event);
	void mouseMove(QMouseEvent* event);
	void paint(QPainter& p);
public:
	QRect rectMask;
protected:
private:
	void changeRectMask(const QPoint& pos);
	void changeMousePosState(const int& x, const int& y);
	void changeMousePosState2(const int& x, const int& y);
	void paintMaskRectInfo(QPainter& p);
	void paintMaskRectBorder(QPainter& p);
	void moveMaskRect(const QPoint& pos);
	void initWinRect();
private:
	QList<QRect> rectWins;
	uint mouseState{ 0 };
	QPoint posPress;
};
