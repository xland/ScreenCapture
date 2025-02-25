#include "ShapeBase.h"
#include "../App/App.h"
#include "../Win/Box.h"
#include "../Win/Canvas.h"

ShapeBase::ShapeBase(QObject* parent) :QObject(parent), box{(Box*)parent}
{
}

ShapeBase::~ShapeBase()
{
}

void ShapeBase::showDragger()
{
	if (state == ShapeState::ready) {
		//避免鼠标在ready的shape上移动的时候不断的重绘
		return;
	}
	if (state != ShapeState::ready) {
		state = ShapeState::ready;
		box->winBoard->refresh();
	}
}

void ShapeBase::paintOnBoard()
{
	if (state != ShapeState::ready) {
		state = ShapeState::ready;
	}
	box->winBoard->refresh();
}

void ShapeBase::painting()
{

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
}

void ShapeBase::paintingPrepare()
{
	//temp状态的shape,鼠标按下
}
