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
	void saveToClipboard();
	void saveToFile();
	void keyPressEvent(QKeyEvent* event) override;
public:
	int x, y, w, h;
	HWND hwnd;
	State state;
	ToolMain* toolMain;
	ToolSub* toolSub;
	Canvas* canvas;
protected:
	virtual QImage getTargetImg()=0;
	void moveCursor(const QPoint& pos);
	void mouseDoubleClickEvent(QMouseEvent* event) override;
private:

};
