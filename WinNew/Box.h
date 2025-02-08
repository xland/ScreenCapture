#pragma once

#include <QObject>
#include "Bg.h"
#include "Canvas.h"
#include "Magnifier.h"
#include "Mask.h"
#include "../App/State.h"

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
		State state{ State::start };
	private:
		void initWins();

	};
}

