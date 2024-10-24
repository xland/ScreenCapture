#pragma once
#include <Windows.h>
#include <string>
#include <memory>
#include <vector>
#include <qwidget.h>
#include <qobject.h>
#include "State.h"

class WinBoard;
class WinCanvas;
class WinMask;
class ToolMain;
class ToolSub;
class ShapeDragger;
class ShapeBase;
class WinFull :public QObject
{
	Q_OBJECT
public:
	WinFull(QObject* parent = nullptr);
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
	void initSize();
	void createNativeWindow();
	void createWidget();
	void createTool();
	void processWidget(QWidget* tar);
	void processTool(QWidget* tar);
	static LRESULT CALLBACK routeWinMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
private:
};

