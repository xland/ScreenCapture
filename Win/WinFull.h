#pragma once
#include "WinBox.h"


class WinCanvas;
class WinMask;
class ToolColorPos;
class ToolMain;
class ToolSub;
class PixelInfo;
class ShapeBase;

class WinFull : public WinBox
{
	Q_OBJECT
public:
	WinFull(QObject* parent = nullptr);
	~WinFull();
	static void init();
	static WinFull* get();
	void showToolMain() override;
	void showToolSub() override;
	void close() override;
public:
	WinMask* winMask;
	ToolColorPos* toolColorPos;
protected:
	void mousePress(QMouseEvent* event) override;
	void mousePressRight(QMouseEvent* event) override;
	void mouseDBClick(QMouseEvent* event) override;
	void mouseMove(QMouseEvent* event) override;
	void mouseDrag(QMouseEvent* event) override;
	void mouseRelease(QMouseEvent* event) override;
private:
	void initWinSizeByDesktopSize();
	void initDesktopImg();
};

