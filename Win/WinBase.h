#pragma once

#include <QWidget>
#include <qimage.h>
#include <QPixmap>
#include "../App/State.h"

class ToolMain;
class ToolSub;
class ShapeBase;
class Canvas;
class Board;
class WinBase  : public QWidget
{
	Q_OBJECT
public:
	WinBase(QWidget* parent = nullptr);
	virtual ~WinBase();
	virtual void showToolMain() = 0;
	virtual void showToolSub() = 0;
	virtual void closeWin() = 0;
	ShapeBase* addShape();
	void updateCursor(Qt::CursorShape cursor);

public:
	State state{ State::start };
	std::vector<ShapeBase*> shapes;
	ToolMain* toolMain;
	ToolSub* toolSub;
	QPixmap bgImg;
	Canvas* canvas;
	Board* board;

private:
	bool eventFilter(QObject* watched, QEvent* event) override;
};
