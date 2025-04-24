#pragma once
#include <Windows.h>
#include <QMainWindow>
#include <QMouseEvent>
#include <QImage>

#include "../App/State.h"

class ToolMain;
class ToolSub;
class Canvas;
class WinBase  : public QMainWindow
{
	Q_OBJECT
public:
	WinBase(QWidget* parent = nullptr);
	~WinBase();
	virtual void saveToClipboard()=0;
	virtual void saveToFile()=0;
public:
	int x, y, w, h;
	HWND hwnd;
	State state;
	ToolMain* toolMain;
	ToolSub* toolSub;
	Canvas* canvas;
protected:
protected:
	virtual void mousePress(QMouseEvent* event){};
	virtual void mousePressRight(QMouseEvent* event) {};
	virtual void mouseDBClick(QMouseEvent* event) {};
	virtual void mouseMove(QMouseEvent* event){};
	virtual void mouseDrag(QMouseEvent* event){};
	virtual void mouseRelease(QMouseEvent* event){};
private:

};
