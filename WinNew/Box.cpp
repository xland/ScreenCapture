#include "Box.h"
#include "Bg.h"
#include "Canvas.h"

namespace Win
{
	Box::Box(QObject* parent) : QObject(parent)
	{
	}

	Box::~Box()
	{
	}

	void Box::init()
	{
		auto bg = new Bg();
		bg->fullWindow();
		auto canvas = new Canvas();
		canvas->fullWindow();
	}
}
