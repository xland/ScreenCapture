#pragma once
#include <Windows.h>
#include <string>
#include <memory>
#include <vector>
#include <qwidget.h>
#include <qobject.h>
#include <qimage.h>

#include "../App/State.h"

class CutMask;
class ToolMain;
class ToolSub;
class ShapeDragger;
class ShapeBase;
class WinFull : public QWidget
{
	Q_OBJECT
public:
	WinFull(QWidget* parent = nullptr);
	~WinFull();
	static void init();
	static void dispose();
	static WinFull* get();
public:
	State state{ State::start };
	HWND hwnd;
	float scaleFactor{1.0f};
	int x, y, w, h;
	ToolMain* toolMain;
	ToolSub* toolSub;
	ShapeDragger* shapeDragger;
	CutMask* cutMask;
	QPixmap bgImg;
	std::vector<QRect> screens;
	std::vector<QRect> winRects;
protected:
	void paintEvent(QPaintEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void showEvent(QShowEvent* event) override;
private:
	void initWinRects();
	void closeNative();
	void initSize();
	void initBgImg();
	void createNativeWindow();
	void initTools();
	void processSubWin();
	void processTool(QWidget* tar);
	void initScreens();
	static LRESULT CALLBACK routeWinMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
private:
};

