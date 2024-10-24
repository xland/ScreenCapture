#pragma once
#include <Windows.h>
#include <string>
#include <memory>
#include <vector>
#include <qwidget.h>

#include "State.h"

class WinBoard;
class WinCanvas;
class WinMask;
class ToolMain;
class ToolSub;
class ShapeDragger;
class ShapeBase;
class WinFull
{
public:
	WinFull();
	~WinFull();
	void close();
	void addShape(const QPoint& pos);
public:
	State state{ State::start };
	ToolMain* toolMain;
	ToolSub* toolSub;
	ShapeDragger* shapeDragger;
	std::vector<ShapeBase*> shapes;
	HWND hwnd;
	int x, y, w, h;
	WinBoard* board;
	WinCanvas* canvas;
	WinMask* mask;
private:
	void createNativeWindow();
	void processWidget(QWidget* tar);
	static LRESULT CALLBACK routeWinMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
private:
};

