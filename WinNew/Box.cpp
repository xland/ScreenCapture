#include <QApplication>
#include "Box.h"


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
		auto box = new Win::Box(qApp);
		box->initWins();
	}
	void Box::initWins()
	{
		bg = new Win::Bg(this);
		bg->fullWindow();
		canvas = new Win::Canvas(this);
		canvas->fullWindow();
		magnifier = new Win::Magnifier(this);


		mask = new Mask(this);
	}
}
