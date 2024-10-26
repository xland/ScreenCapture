#pragma once
#include <Windows.h>
#include <string>
#include <memory>
#include <vector>
#include <qwidget.h>
#include <qobject.h>
#include <qimage.h>
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
	void init();
	void close();
public:
	State state{ State::start };
	ToolMain* toolMain;
	ToolSub* toolSub;
	ShapeDragger* shapeDragger;
	HWND hwnd;
	int x, y, w, h;
	WinBoard* board;
	WinCanvas* canvas;
	WinMask* mask;
	QPixmap bgImg;
	std::vector<QRect> screens;
private:
	void initSize();
	void initBgImg();
	void createNativeWindow();
	void createWidget();
	void createTool();
	void processWidget(QWidget* tar);
	void processTool(QWidget* tar);
	void initScreens();
	static LRESULT CALLBACK routeWinMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
private:
};

