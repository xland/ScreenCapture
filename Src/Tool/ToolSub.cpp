#include "pch.h"
#include <Windows.UI.Composition.Interop.h> 
#include "../Win/WinPin.h"
#include "ToolSub.h"
#include "ToolMain.h"

using namespace Microsoft::WRL;

ToolSub::ToolSub(WinPin* win) :Ling::WinBase(), win(win)
{
	//btnSize *= dpi;
	//sliderSize *= dpi;
	marginTop *= dpi;
	createNativeWindow(WS_EX_TOPMOST | WS_EX_NOACTIVATE, WS_POPUP);
	//win->onMoved.add([this, win]() {
	//	this->setPosition(win->x, win->y + win->h + 5.f * win->dpi);
	//});
}

ToolSub::~ToolSub()
{
}

void ToolSub::setPosAndShow()
{
	arrowX = win->toolMain->getBtnCenterX();
	int x, y, w, h;
	if (win->toolMain->curId == L"mosaic" || win->toolMain->curId == L"eraser") {
		y = win->toolMain->y + win->toolMain->h;
		h = btnSize + marginTop/dpi;
		w = btnSize + sliderSize;
		x = win->toolMain->x + arrowX - w*dpi / 2.f;
		setPosition(x, y + 2);
		setSize(w, h);
		arrowX = this->w / 2.f ;
	}
	else {
		x = win->toolMain->x;
		y = win->toolMain->y + win->toolMain->h;
		h = btnSize + marginTop;
		w = btnSize*10 + sliderSize;
		setPosition(x, y + 2);
		setSize(w, h);
	}
	if (!isVisible) {
		show();
		isVisible = true;
	}
	else {
		refresh();
	}
	
}

void ToolSub::onCreated()
{
	auto d2d = Ling::D2D::get();
	d2d->deviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), brushBg.GetAddressOf());
	d2d->deviceContext->CreateSolidColorBrush(D2D1::ColorF(0xA8A8A8), brushBorder.GetAddressOf());
}

void ToolSub::layout()
{
	Ling::WinBase::layout();

	const int pxW = static_cast<int>(w);
	const int pxH = static_cast<int>(h);
	if (pxW <= 0 || pxH <= 0) return;
	if (!surface) {
		auto d2d = Ling::D2D::get();
		surface = d2d->createDrawingSurface(compositor, (float)pxW, (float)pxH);
		auto brush = compositor.CreateSurfaceBrush(surface);
		brush.Stretch(winrt::Windows::UI::Composition::CompositionStretch::None);
		body->visual.Brush(brush);
	}
	else {
		auto sz = surface.SizeInt32();
		if (sz.Width != pxW || sz.Height != pxH) {
			surface.Resize({ pxW, pxH });
		}
	}

	auto s = surface.as<ABI::Windows::UI::Composition::ICompositionDrawingSurfaceInterop>();
	ComPtr<ID2D1DeviceContext> ctx;
	POINT offset{};
	s->BeginDraw(nullptr, __uuidof(ID2D1DeviceContext), reinterpret_cast<void**>(ctx.GetAddressOf()), &offset);
	auto trans = D2D1::Matrix3x2F::Translation((float)offset.x, (float)offset.y);
	ctx->SetTransform(trans);
	ctx->Clear(0);
	
	auto d2d = Ling::D2D::get();
	d2d->d2dFactory->CreatePathGeometry(borderPath.GetAddressOf());
	ComPtr<ID2D1GeometrySink> sink;
	borderPath->Open(sink.GetAddressOf());
	auto borderW{ dpi };
	sink->BeginFigure({ borderW,marginTop }, D2D1_FIGURE_BEGIN_FILLED);
	D2D1_POINT_2F points[6] = {
		D2D1_POINT_2F{arrowX - 3.f * dpi,marginTop},
		D2D1_POINT_2F{arrowX,dpi},
		D2D1_POINT_2F{arrowX + 3.f * dpi,marginTop},
		D2D1_POINT_2F{(float)w - borderW,marginTop},
		D2D1_POINT_2F{(float)w - borderW,(float)h - borderW},
		D2D1_POINT_2F{borderW,(float)h - borderW}
	};
	sink->AddLines(points, 6);
	sink->EndFigure(D2D1_FIGURE_END_CLOSED);
	sink->Close();

	ctx->FillGeometry(borderPath.Get(), brushBg.Get());
	ctx->DrawGeometry(borderPath.Get(), brushBorder.Get(), dpi);

	s->EndDraw();
}

void ToolSub::onMinMaxInfo(MINMAXINFO* mmi)
{
	mmi->ptMinTrackSize.x = 1;
	mmi->ptMinTrackSize.y = 1;
}