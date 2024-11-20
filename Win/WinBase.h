#pragma once

#include <QWidget>
#include <qimage.h>
#include <QPixmap>
#include <QMouseEvent>
#include <QMouseEvent>

#include "../App/State.h"

class ToolMain;
class ToolSub;
class ShapeBase;
class Canvas;
class Board;
class WinBg;
class WinBase  : public QWidget
{
	Q_OBJECT
public:
	WinBase(QWidget* parent = nullptr);
	virtual ~WinBase();
	virtual void showToolMain() {};
	virtual void showToolSub() {};
	virtual void closeWin() = 0;
	ShapeBase* addShape();
	void updateCursor(Qt::CursorShape cursor);
	virtual void mousePress(QMouseEvent* event) = 0;
	virtual void mouseDrag(QMouseEvent* event) = 0;
	virtual void mouseRelease(QMouseEvent* event) = 0;
	virtual void mouseMove(QMouseEvent* event) = 0;
public:
	State state{ State::start };
	std::vector<ShapeBase*> shapes;
	ToolMain* toolMain;
	ToolSub* toolSub;
	Canvas* canvas;
	Board* board;
	WinBg* winBg;
protected:
	//void mousePressEvent(QMouseEvent* event) override;
private:
	bool eventFilter(QObject* watched, QEvent* event) override;
};
