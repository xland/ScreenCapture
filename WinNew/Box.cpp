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
	void Box::showToolMain()
	{
		//if (!toolMain) {
		//	toolMain = new ToolMain(this);
		//}
		//toolMain->setBtnEnable(QString{ "pin" }, true);
		//toolMain->confirmPos();
		//toolMain->show();
	}

	void Box::showToolSub()
	{
		//if (!toolSub) {
		//	toolSub = new ToolSub(this);
		//}
		//if (toolSub->isVisible()) {
		//	toolSub->hide();
		//}
		//toolSub->show();
	}
	void Box::hideTools(State state)
	{
		//if (toolMain) {
		//	toolMain->hide();
		//}
		//if (toolSub) {
		//	toolSub->hide();
		//}
		//if (canvas->curShape) {
		//	winCanvas->clear();
		//}
		//this->state = state;
		//if (state == State::start) {
		//	if (!magnifier) {
		//		magnifier = new Magnifier(this);
		//	}
		//}
	}
}
