#include "pch.h"
#include "../Win/WinPin.h"
#include "ShapeBase.h"

ShapeBase::ShapeBase(WinPin* win):win{win}, draggerSize{6*win->dpi}
{
	win->render->CreateSolidColorBrush(D2D1::ColorF(0x000000), brushDragger.GetAddressOf());
}

ShapeBase::~ShapeBase()
{}