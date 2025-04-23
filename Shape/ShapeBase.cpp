#include "ShapeBase.h"
#include "../App/App.h"
#include "../Win/WinBase.h"

ShapeBase::ShapeBase(QObject* parent):QObject(parent)
{
}

ShapeBase::~ShapeBase()
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
