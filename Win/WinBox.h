#pragma once

#include <QWidget>
#include "WinBase.h"

class ToolMain;
class ToolSub;
class PixelInfo;
class WinBoard;
class WinCanvas;
class ShapeBase;
class WinBox : public WinBase
{
	Q_OBJECT

public:
	WinBox(QObject* parent = nullptr);
	virtual ~WinBox();
	void updateCursor(const Qt::CursorShape& cur);
	void refreshBoard();
	virtual void showToolMain()=0;
	virtual void showToolSub()=0;
public:
	State state{ State::start };
	QList<ShapeBase*> shapes;
	ToolMain* toolMain;
	ToolSub* toolSub;
	PixelInfo* pixelInfo;
	WinCanvas* winCanvas;
	WinBoard* winBoard;
protected:
	ShapeBase* addShape();
	void mousePressOnShape(QMouseEvent* event);
	void mouseMoveOnShape(QMouseEvent* event);
	void mouseDragOnShape(QMouseEvent* event);
	void mouseReleaseOnShape(QMouseEvent* event);
};
