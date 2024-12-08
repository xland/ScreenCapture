#pragma once
#include <Windows.h>
#include <QWidget>
#include <QMouseEvent>
#include <QImage>

#include "../App/State.h"


class WinBase  : public QObject
{
	Q_OBJECT
public:
	WinBase(QObject* parent = nullptr);
	virtual ~WinBase();
	void show();
	QImage grab() { return QImage(); };
	QImage grab(const QRect& rect) { return QImage(); };
public:
	int x, y, w, h;
	QImage img;
	qreal dpr{ 1.0 };
	HWND hwnd;
protected:
	void initWindow();
	static LRESULT CALLBACK RouteWinMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	virtual void mousePress(QMouseEvent* event) = 0;
	virtual void mousePressRight(QMouseEvent* event) {};
	virtual void mouseDBClick(QMouseEvent* event) {};
	virtual void mouseMove(QMouseEvent* event) = 0;
	virtual void mouseDrag(QMouseEvent* event) = 0;
	virtual void mouseRelease(QMouseEvent* event) = 0;
private:
	void paint();
	static QMouseEvent createMouseEvent(LPARAM& lParam);
};
