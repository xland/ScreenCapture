#pragma once

#include <QWidget>
#include "../App/State.h"
class ToolMain;
class ToolSub;
class WinBase  : public QWidget
{
	Q_OBJECT
public:
	WinBase(QWidget* parent = nullptr);
	~WinBase();
public:
	State state{ State::start };
	ToolMain* toolMain;
	ToolSub* toolSub;
};
