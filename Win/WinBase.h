#pragma once
#include <Windows.h>
#include <QMainWindow>
#include <QMouseEvent>
#include <QImage>
#include <QTimer>

#include "../App/State.h"

class ToolMain;
class ToolSub;
class Canvas;
class ShapeBase;
class WinBase  : public QMainWindow
{
	Q_OBJECT
public:
	WinBase(QWidget* parent = nullptr);
	~WinBase();
	void initImg();
	void releaseImg();
	void saveToClipboard() {};
	void saveToFile() {};
	void undo() {};
	void redo() {};
	QImage grab() { return QImage(); };
	QImage grab(const QRect& rect);
	void raise();
	void move(const int& x, const int& y);
	void setCurShape(ShapeBase* shape);
	virtual void close();
public:
	int x, y, w, h;
	QImage imgBg;
	QImage imgBoard;
	QImage imgCanvas;
	HWND hwnd;
	State state;
	ToolMain* toolMain;
	ToolSub* toolSub;
	Canvas* canvas;
protected:
	ShapeBase* shapeCur;
protected:
	void initWindow();
	void paintEvent(QPaintEvent* event) override;
	virtual void mousePress(QMouseEvent* event){};
	virtual void mousePressRight(QMouseEvent* event) {};
	virtual void mouseDBClick(QMouseEvent* event) {};
	virtual void mouseMove(QMouseEvent* event){};
	virtual void mouseDrag(QMouseEvent* event){};
	virtual void mouseRelease(QMouseEvent* event){};
private:
	QTimer* timerDragger;
};
