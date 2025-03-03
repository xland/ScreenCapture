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
