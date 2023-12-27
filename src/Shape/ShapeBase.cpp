#include "ShapeBase.h"

static int idNum{ 0 };

ShapeBase::ShapeBase(const int& x, const int& y):startX{x},startY{y}
{
	id = idNum;
	idNum += 1;
}

ShapeBase::~ShapeBase()
{
}
