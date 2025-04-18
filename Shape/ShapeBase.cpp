#include "ShapeBase.h"
#include "../App/App.h"
#include "../Win/WinBase.h"
#include "../Win/WinCanvas.h"

ShapeBase::ShapeBase(QObject* parent):QObject(parent)
{
}

ShapeBase::~ShapeBase()
{
}

void ShapeBase::showDragger()
{
	//auto win = (WinBase*)parent();
	
}

void ShapeBase::paintOnBoard()
{
	if (state != ShapeState::ready) {
		state = ShapeState::ready;
	}
}

void ShapeBase::painting()
{
	auto win = (WinBase*)parent();
	//win->winCanvas->paintShape();
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
	//ready状态的shape，鼠标按下
	auto win = (WinBase*)parent();
	//win->winCanvas->initImg();
	//win->winCanvas->curShape = this;
	//win->winCanvas->paintShape();
}

void ShapeBase::paintingPrepare()
{
	//temp状态的shape,鼠标按下
	auto win = (WinBase*)parent();
	//win->winCanvas->initImg();
	//win->winCanvas->curShape = this;
}
