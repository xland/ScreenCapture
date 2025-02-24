#pragma once

#include <QObject>
#include "../App/State.h"
#include "Bg.h"
#include "Canvas.h"
#include "Magnifier.h"
#include "Mask.h"

class ToolMain;
class ToolSub;
class Box : public QObject
{
	Q_OBJECT
	public:
		Box(QObject* parent);
		~Box();
		static void init();
		void showToolMain();
		void showToolSub();
		void hideTools(State state = State::start);
		void undo();
		void redo();
	public:
		Bg* bg;
		Canvas* canvas;
		Magnifier* magnifier;
		Mask* mask;
		ToolMain* toolMain;
		ToolSub* toolSub;
		State state{ State::start };
	private:
		void initWins();
};