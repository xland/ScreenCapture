#include "pch.h"
#include <Windows.UI.Composition.Interop.h> 
#include "../Win/WinPin.h"
#include "ToolSub.h"
#include "ToolMain.h"

using namespace Microsoft::WRL;

ToolSub::ToolSub(WinPin* win) :Ling::WinBase(), win(win)
{
	btnSize *= dpi;
	sliderSize *= dpi;
	marginTop *= dpi;
	createNativeWindow(WS_EX_TOPMOST | WS_EX_NOACTIVATE, WS_POPUP);
	//win->onMoved.add([this, win]() {
	//	this->setPosition(win->x, win->y + win->h + 5.f * win->dpi);
	//});
}

ToolSub::~ToolSub()
{
}

void ToolSub::showRectTools()
{
	auto btnCenterX = win->toolMain->getBtnCenterX();
	float pxW, pxH;
	int px, py;
	pxW = btnSize * 10 + sliderSize + 6 * dpi;
	pxH = btnSize + marginTop;
	px = win->toolMain->x;
	py = win->toolMain->y + win->toolMain->h;
	arrowX = btnCenterX;
	setPosition(px, py + 2);
	setSize(pxW / dpi, pxH / dpi);
	if (!isVisible) {
		show();
		isVisible = true;
	}
	else {
		refresh();
	}

	auto btn = contentNode->makeChild<Ling::Button>();
	btn->setText(L"\ue602");
	btn->setHeightPercent(100.f);
	btn->setFlexGrow(1.f);
	btn->setHoverBg(0xF2F2F2ff);
	btn->setFontFamily(L"icon");
	btn->setFontSize(13.f);

	auto spliter = contentNode->makeChild<Ling::Slider>();
	spliter->setWidth(100.f);
	spliter->setMarginLeft(3.f);
	spliter->setMarginRight(3.f);
	spliter->setHeightPercent(100.f);
	spliter->setThumbColor(0x888888FF);
	spliter->setHoverThumbColor(0x888888FF);
	spliter->setTrackColor(0x888888FF);
	spliter->setFillColor(0x888888FF);



	for (size_t i = 0; i < colors.size(); i++)
	{
		auto btn = contentNode->makeChild<Ling::Button>();
		btn->setHeightPercent(100.f);
		btn->setFlexGrow(1.f);
		btn->setAlignItems(Ling::Align::Center);
		btn->setJustifyContent(Ling::Justify::Center);
		btn->setHoverBg(0XF2F2F2ff);
		btn->onClick.add([this](Ling::Button* btn) {this->onColorSelect(btn);});
		colorBtns.push_back(btn);

		auto label = btn->makeChild<Ling::Label>();
		label->setAlignItems(Ling::Align::Center);
		label->setJustifyContent(Ling::Justify::Center);
		label->setSize(13.f, 13.f);
		if (i == 0) {
			label->setText(L"\ue6ad");
		}
		label->setFontFamily(L"icon");
		label->setFontSize(8.f);
		label->setBg(colors[i]);
		label->setBorderRadius(2.f);
		if (i == colors.size() - 1) {
			label->setColor(0x000000FF);
			label->setBorder(1.f, 0xA8A8A8FF);
		}
		else {
			label->setColor(0xFFFFFFFF);
		}
	}
}

void ToolSub::setPosAndShow()
{
	auto btnCenterX = win->toolMain->getBtnCenterX();
	float pxW, pxH;
	int px, py;
	if (win->toolMain->curId == L"mosaic" || win->toolMain->curId == L"eraser") {
		pxW = btnSize + sliderSize + 6 * dpi;
		pxH = btnSize + marginTop;
		px = int(win->toolMain->x + btnCenterX - pxW / 2.f);
		py = win->toolMain->y + win->toolMain->h;
		arrowX = pxW / 2.f;
	}
	else {
		pxW = btnSize * 10 + sliderSize + 6*dpi;
		pxH = btnSize + marginTop;
		px = win->toolMain->x;
		py = win->toolMain->y + win->toolMain->h;
		arrowX = btnCenterX;
	}
	setPosition(px, py + 2);
	setSize(pxW / dpi, pxH / dpi);
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
	contentNode = body->makeChild<Ling::Node>();
	contentNode->setPositionType(Ling::Position::Absolute);
	contentNode->setPosition(Ling::Edge::Left, 1);
	contentNode->setPosition(Ling::Edge::Top, std::ceil(marginTop / dpi));
	contentNode->setPosition(Ling::Edge::Right, 2);
	contentNode->setPosition(Ling::Edge::Bottom, 2);
	contentNode->setFlexDirection(Ling::FlexDirection::Row);	
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
	paintBorder(ctx.Get());
	s->EndDraw();
}

void ToolSub::onMinMaxInfo(MINMAXINFO* mmi)
{
	mmi->ptMinTrackSize.x = 1;
	mmi->ptMinTrackSize.y = 1;
}

void ToolSub::paintBorder(ID2D1DeviceContext* ctx)
{
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
	ComPtr<ID2D1SolidColorBrush> brushBg;
	ComPtr<ID2D1SolidColorBrush> brushBorder;
	ctx->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), brushBg.GetAddressOf());
	ctx->CreateSolidColorBrush(D2D1::ColorF(0xA8A8A8), brushBorder.GetAddressOf());
	ctx->FillGeometry(borderPath.Get(), brushBg.Get());
	ctx->DrawGeometry(borderPath.Get(), brushBorder.Get(), dpi);
}

void ToolSub::onColorSelect(Ling::Button* btn)
{
	if (colorBtns[selectColorIndex] == btn) return;
	auto label = dynamic_cast<Ling::Label*>(colorBtns[selectColorIndex]->children[1].get());
	label->setText(L"");
	label = dynamic_cast<Ling::Label*>(btn->children[1].get());
	label->setText(L"\ue6ad");
	for (size_t i = 0; i < colorBtns.size(); i++)
	{
		if (colorBtns[i] == btn) {
			selectColorIndex = i;
			break;
		}
	}
}
