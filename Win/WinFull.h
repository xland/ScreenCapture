#pragma once
#include <Windows.h>
#include <string>
#include <memory>
#include <vector>
#include <qwidget.h>
#include <qobject.h>
#include <qimage.h>

#include "../App/State.h"

class LayerBoard;
class LayerCanvas;
class LayerMask;
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
	int x, y, w, h;
	ToolMain* toolMain;
	ToolSub* toolSub;
	ShapeDragger* shapeDragger;
	LayerBoard* layerBoard;
	LayerCanvas* layerCanvas;
	LayerMask* layerMask;
	QImage bgImg;
	std::vector<QRect> screens;
protected:
	void paintEvent(QPaintEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void showEvent(QShowEvent* event) override;
private:
	void closeNative();
	void initSize();
	void initBgImg();
	void createNativeWindow();
	void initLayers();
	void createTool();
	void processWidget();
	void processTool(QWidget* tar);
	void initScreens();
	static LRESULT CALLBACK routeWinMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
private:
};

