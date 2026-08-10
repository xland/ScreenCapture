#include "pch.h"
#include "App.h"
#include "../Win/WinPin.h"
#include "ShapeBase.h"

ShapeBase::ShapeBase(WinPin* win):win{win}, draggerSize{6*win->dpi}
{
	auto d2d = Ling::D2D::get();
	d2d->deviceContext->CreateSolidColorBrush(D2D1::ColorF(0x000000), brushDragger.GetAddressOf());
}

ShapeBase::~ShapeBase()
{}

bool ShapeBase::isInRect(const D2D1_RECT_F rect, const float x, const float y)
{
	return (x > rect.left && x<rect.right && y>rect.top && y < rect.bottom);
}
