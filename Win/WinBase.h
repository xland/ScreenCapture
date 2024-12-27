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
	void initImg();
	void releaseImg();
	QImage grab() { return QImage(); };
	QImage grab(const QRect& rect);
	void raise();
	void move(const int& x, const int& y);
	virtual void close();
public:
	int x, y, w, h;
	QImage img;
	HWND hwnd;
protected:
	void initWindow(bool isTransparent=true);
	void paint();

	virtual void mousePress(QMouseEvent* event){};
	virtual void mousePressRight(QMouseEvent* event) {};
	virtual void keyEscPress() {};
	virtual void ctrlTPress() {};
	virtual void mouseDBClick(QMouseEvent* event) {};
	virtual void mouseMove(QMouseEvent* event){};
	virtual void mouseDrag(QMouseEvent* event){};
	virtual void mouseRelease(QMouseEvent* event){};
	virtual void removeShape() {};
	virtual void moveByKey(const int& key) {};
	virtual void copyColor(const int& key) {};
private:
	static LRESULT CALLBACK routeWinMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK processWinMsg(UINT msg, WPARAM wParam, LPARAM lParam);
	bool processKeyDown(WPARAM wParam);
};
