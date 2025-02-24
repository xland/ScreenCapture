#include <QApplication>
#include "Box.h"
#include "Tool/ToolMain.h"
#include "Tool/ToolSub.h"


Box::Box(QObject* parent) : QObject(parent)
{
}
Box::~Box()
{
}
void Box::init()
{
	auto box = new Box(qApp);
	box->initWins();
}
void Box::initWins()
{
	bg = new Bg(this);
	bg->fullWindow();
	canvas = new Canvas(this);
	canvas->fullWindow();
	magnifier = new Magnifier(this);
	mask = new Mask(this);
}
void Box::showToolMain()
{
	if (!toolMain) {
		toolMain = new ToolMain(this);
	}
	toolMain->setBtnEnable(QString{ "pin" }, true);
	toolMain->confirmPos();
	toolMain->show();
}

void Box::showToolSub()
{
	if (!toolSub) {
		toolSub = new ToolSub(this);
	}
	if (toolSub->isVisible()) {
		toolSub->hide();
	}
	toolSub->show();
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

void Box::undo()
{
}

void Box::redo()
{
}

void Box::saveToClipboard()
{
}

void Box::saveToFile()
{
}

void Box::moveByKey(const int& key)
{
}

void Box::removeShape()
{
}

void Box::escPress()
{
}

void Box::copyColor(const int& key)
{
}
