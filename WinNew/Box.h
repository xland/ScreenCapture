#pragma once

#include <QObject>
#include "../App/State.h"
#include "../Tool/ToolMain.h"
#include "../Tool/ToolSub.h"
#include "Bg.h"
#include "Canvas.h"
#include "Magnifier.h"
#include "Mask.h"


namespace Win
{
	class Box : public QObject
	{
		Q_OBJECT
	public:
		Box(QObject* parent);
		~Box();
		static void init();
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
}

