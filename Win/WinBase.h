#pragma once

#include <QWidget>
#include <qimage.h>
#include <QPixmap>
#include "../App/State.h"

class ToolMain;
class ToolSub;
class ShapeDragger;
class WinBase  : public QWidget
{
	Q_OBJECT
public:
	WinBase(QWidget* parent = nullptr);
	virtual ~WinBase();
	virtual void showToolMain() = 0;
	virtual void showToolSub() = 0;
	virtual void closeWin() = 0;
public:
	State state{ State::start };
	ToolMain* toolMain;
	ToolSub* toolSub;
	ShapeDragger* shapeDragger;
	QPixmap bgImg;
};
