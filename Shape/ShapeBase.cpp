#include "ShapeBase.h"
#include "../App/App.h"
#include "../Win/WinBox.h"
#include "../Win/WinCanvas.h"
#include "../Win/WinBoard.h"

ShapeBase::ShapeBase(QObject* parent):QObject(parent)
{
	//App::getFullCanvas()->curShape = this;
}

ShapeBase::~ShapeBase()
{
}

void ShapeBase::showDragger()
{
	auto win = (WinBox*)parent();
	if (state != ShapeState::ready) {
		state = ShapeState::ready;
		win->winBoard->refresh();
	}
	win->winCanvas->curShape = this;
	win->winCanvas->paintDragger();
}

void ShapeBase::painting()
{
	auto win = (WinBox*)parent();
	win->winCanvas->paintShape();
}

void ShapeBase::prepareDraggers(const int& size)
{
	if (draggers.empty()) {
		for (int i = 0; i < size; i++)
		{
			draggers.push_back(QRect());
		}
	}
}

void ShapeBase::paintingStart()
{
	auto win = (WinBox*)parent();
	win->winCanvas->initImg();
	win->winCanvas->curShape = this;
	win->winCanvas->paintShape();
	win->winBoard->refresh();
}
