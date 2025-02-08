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
		box->bg = new Win::Bg(box);
		box->bg->fullWindow();
		box->canvas = new Win::Canvas(box);
		box->canvas->fullWindow();
		box->magnifier = new Win::Magnifier(box);
	}
}
