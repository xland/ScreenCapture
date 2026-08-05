#include "pch.h"
#include <Windows.UI.Composition.Interop.h> 
#include "../Tool/ToolMain.h"
#include "WinPin.h"
#include "WinCap.h"
#include "../App.h"

using namespace Microsoft::WRL;
namespace {
	std::vector<std::unique_ptr<WinPin>> winPins;
}

WinPin::WinPin(int x, int y, int w, int h) : Ling::WinBase()
{
	this->x = x;
	this->y = y;
	this->w = w;
	this->h = h;
    screenImg = WinCap::get()->getCutImg();
	toolMain = std::make_unique<ToolMain>(this);
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
    auto d2d = Ling::D2D::get();
    surface = d2d->createDrawingSurface(compositor, (float)w, (float)h);
    auto brush = compositor.CreateSurfaceBrush(surface);
    brush.Stretch(winrt::Windows::UI::Composition::CompositionStretch::None);
    body->visual.Brush(brush);
    d2d->deviceContext->CreateSolidColorBrush(D2D1::ColorF(0x1677ff), borderBrush.GetAddressOf());
    show();
}

void WinPin::layout()
{
    Ling::WinBase::layout();
    if (!screenImg) return;
    auto s = surface.as<ABI::Windows::UI::Composition::ICompositionDrawingSurfaceInterop>();
    ComPtr<ID2D1DeviceContext> ctx;
    POINT offset{};
    s->BeginDraw(nullptr, __uuidof(ID2D1DeviceContext), reinterpret_cast<void**>(ctx.GetAddressOf()), &offset);
    auto trans = D2D1::Matrix3x2F::Translation((float)offset.x, (float)offset.y);
    ctx->SetTransform(trans);
    auto sz = screenImg->GetSize();
    D2D1_RECT_F destRect = D2D1::RectF(0, 0, sz.width, sz.height);
    ctx->DrawBitmap(screenImg.Get(), destRect);
	ctx->DrawRectangle(destRect, borderBrush.Get(), 2*dpi);
    s->EndDraw();
}

LRESULT WinPin::onHitTest(const POINT pos)
{
    return HTCAPTION;
}

void WinPin::onMinMaxInfo(MINMAXINFO* mmi)
{
	auto [x, y, w, h] = App::get()->getScreenArea();
	mmi->ptMaxPosition.x = x;
	mmi->ptMaxPosition.y = y;
	mmi->ptMaxSize.x = w;
	mmi->ptMaxSize.y = h;
	mmi->ptMinTrackSize.x = 1;
	mmi->ptMinTrackSize.y = 1;
}
