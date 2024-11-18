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
class LayerBase : public QWidget
{
	Q_OBJECT
public:
	LayerBase(QWidget* parent = nullptr);
	virtual ~LayerBase();
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
};
