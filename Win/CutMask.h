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
	void mouseMove(QMouseEvent* event);
	void paint(QPainter& p);
public:
	QRect rectMask;
protected:
private:
	void changeRectMask(const QPoint& pos);
	void changeMouseState(const int& x, const int& y);
	void moveMaskRect(const QPoint& pos);
	void initWinRect();
private:
	QList<QRect> rectWins;
	uint mouseState{ 0 };
	QPoint posPress;
	float maskStroke{ 1.5 };
};
