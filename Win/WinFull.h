#pragma once
#include <QWidget>
#include <QObject>

#include "WinBase.h"


class WinCanvas;
class WinMask;
class ToolColorPos;
class ToolMain;
class ToolSub;
class ShapeBase;

class WinFull : public WinBase
{
	Q_OBJECT
public:
	WinFull(QWidget* parent = nullptr);
	~WinFull();
	static void init();
	static void dispose();
	static WinFull* get();
	void showToolMain() override;
	void showToolSub() override;
	void closeWin() override;
public:
	WinMask* winMask;
	ToolColorPos* toolColorPos;
protected:
	void paintEvent(QPaintEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
private:
	void initWinSizeByDesktopSize();
	void initDesktopImg();
};

