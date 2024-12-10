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
	void releaseImg();
	QImage grab() { return QImage(); };
	QImage grab(const QRect& rect);
public:
	int x, y, w, h;
	QImage img;
	HWND hwnd;
protected:
	void initWindow(bool isTransparent=true);
	void paint();
	static LRESULT CALLBACK RouteWinMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void initSizeByWin(WinBase* win);

	virtual void mousePress(QMouseEvent* event) = 0;
	virtual void mousePressRight(QMouseEvent* event) {};
	virtual void mouseDBClick(QMouseEvent* event) {};
	virtual void mouseMove(QMouseEvent* event) = 0;
	virtual void mouseDrag(QMouseEvent* event) = 0;
	virtual void mouseRelease(QMouseEvent* event) = 0;
private:
	static QMouseEvent createMouseEvent(const LPARAM& lParam,const QEvent::Type& type,const Qt::MouseButton& btn = Qt::MouseButton::LeftButton);
};
