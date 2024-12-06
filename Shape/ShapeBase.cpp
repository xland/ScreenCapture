#include "ShapeBase.h"
#include "../App/App.h"
//#include "WinCanvas.h"

ShapeBase::ShapeBase(QObject* parent):QObject(parent)
{
	//App::getFullCanvas()->curShape = this;
}

ShapeBase::~ShapeBase()
{
}
