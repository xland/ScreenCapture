#include "pch.h"
#include "WinPin.h"
#include "../App.h"

namespace {
	std::vector<std::unique_ptr<WinPin>> winPins;
}


WinPin::WinPin(int x, int y, int w, int h) : Ling::WinBase()
{
	this->x = x;
	this->y = y;
	this->w = w;
	this->h = h;
}

WinPin::~WinPin()
{
}

void WinPin::init(int x, int y, int w, int h)
{
	auto ptr = new WinPin(x,y,w,h);
	std::unique_ptr<WinPin> winPin{ ptr };
	ptr->createNativeWindow(WS_EX_TOPMOST, WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_POPUP);
	winPins.push_back(std::move(winPin));
}

void WinPin::onCreated()
{
	enableShadow();
    App::get()->takeScreenShot(x, y, w, h, &screenImg);
    auto d2d = Ling::D2D::get();
    surface = d2d->createDrawingSurface(compositor, (float)w, (float)h);
    auto brush = compositor.CreateSurfaceBrush(surface);
    brush.Stretch(winrt::Windows::UI::Composition::CompositionStretch::None);
    body->visual.Brush(brush);
    show();
}

void WinPin::layout()
{
}
