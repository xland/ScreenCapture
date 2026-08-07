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
	sliderMargin *= dpi;
	contentInsetX *= dpi;
	marginTop *= dpi;
	createNativeWindow(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, WS_POPUP);
}

ToolSub::~ToolSub()
{
}

void ToolSub::showRectTools()
{
	contentNode->removeAllChildren();
	initSize(1, true);

	auto btn = contentNode->makeChild<Ling::Button>();
	btn->setText(L"\ue602");
	btn->setHeightPercent(100.f);
	btn->setFlexGrow(1.f);
	btn->setHoverBg(0xF2F2F2ff);
	btn->setFontFamily(L"icon");
	btn->setFontSize(13.f);
	initSlider();
	initColorBtns();
}

void ToolSub::showEllipseTools()
{
	contentNode->removeAllChildren();
	initSize(1, true);

	auto btn = contentNode->makeChild<Ling::Button>();
	btn->setText(L"\ue600");
	btn->setHeightPercent(100.f);
	btn->setFlexGrow(1.f);
	btn->setHoverBg(0xF2F2F2ff);
	btn->setFontFamily(L"icon");
	btn->setFontSize(13.f);

	initSlider();
	initColorBtns();
}

void ToolSub::showArrowTools()
{
	contentNode->removeAllChildren();
	initSize(1, true);
	auto btn = contentNode->makeChild<Ling::Button>();
	btn->setText(L"\ue604");
	btn->setHeightPercent(100.f);
	btn->setFlexGrow(1.f);
	btn->setBg(0xe6f4ffff);
	btn->setHoverBg(0xe6f4ffff);
	btn->setFontFamily(L"icon");
	btn->setFontSize(13.f);
	initSlider();
	initColorBtns();
}

void ToolSub::showNumberTools()
{
	contentNode->removeAllChildren();
	initSize(1, true);
	auto btn = contentNode->makeChild<Ling::Button>();
	btn->setText(L"\ue605");
	btn->setHeightPercent(100.f);
	btn->setFlexGrow(1.f);
	btn->setBg(0xe6f4ffff);
	btn->setHoverBg(0xe6f4ffff);
	btn->setFontFamily(L"icon");
	btn->setFontSize(13.f);
	initSlider();
	initColorBtns();
}

void ToolSub::showLineTools()
{
	contentNode->removeAllChildren();
	initSize(1, true);
	auto btn = contentNode->makeChild<Ling::Button>();
	btn->setText(L"\ue607");
	btn->setHeightPercent(100.f);
	btn->setFlexGrow(1.f);
	btn->setHoverBg(0xF2F2F2ff);
	btn->setFontFamily(L"icon");
	btn->setFontSize(13.f);
	initSlider();
	initColorBtns();
}

void ToolSub::showTextTools()
{
	contentNode->removeAllChildren();
	initSize(2, true);
	auto btn = contentNode->makeChild<Ling::Button>();
	btn->setText(L"\ue634");
	btn->setHeightPercent(100.f);
	btn->setFlexGrow(1.f);
	btn->setHoverBg(0xF2F2F2ff);
	btn->setFontFamily(L"icon");
	btn->setFontSize(13.f);

	auto btn2 = contentNode->makeChild<Ling::Button>();
	btn2->setText(L"\ue682");
	btn2->setHeightPercent(100.f);
	btn2->setFlexGrow(1.f);
	btn2->setHoverBg(0xF2F2F2ff);
	btn2->setFontFamily(L"icon");
	btn2->setFontSize(13.f);
	initSlider();
	initColorBtns();
}

void ToolSub::showMosaicTools()
{
	contentNode->removeAllChildren();
	initSize(1, false, true);
	auto btn = contentNode->makeChild<Ling::Button>();
	btn->setText(L"\ue602");
	btn->setHeightPercent(100.f);
	btn->setFlexGrow(1.f);
	btn->setHoverBg(0xF2F2F2ff);
	btn->setFontFamily(L"icon");
	btn->setFontSize(13.f);
	initSlider();
}

void ToolSub::showEraserTools()
{
	contentNode->removeAllChildren();
	initSize(1, false, true);
	auto btn = contentNode->makeChild<Ling::Button>();
	btn->setText(L"\ue602");
	btn->setHeightPercent(100.f);
	btn->setFlexGrow(1.f);
	btn->setHoverBg(0xF2F2F2ff);
	btn->setFontFamily(L"icon");
	btn->setFontSize(13.f);
	initSlider();
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

void ToolSub::initColorBtns()
{
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

void ToolSub::initSlider()
{
	auto slider = contentNode->makeChild<Ling::Slider>();
	slider->setWidth(100.f);
	slider->setMarginLeft(3.f);
	slider->setMarginRight(3.f);
	slider->setHeightPercent(100.f);
	slider->setThumbColor(0x888888FF);
	slider->setHoverThumbColor(0x888888FF);
	slider->setTrackColor(0x888888FF);
	slider->setFillColor(0x888888FF);
}

float ToolSub::getDesiredHeight()
{
	return btnSize + marginTop;
}

// 宽度 = 工具按钮 + 颜色按钮 + 滑块（含左右 margin）+ contentNode 左右内边距。
// 之前这里漏算了滑块的真实宽度和内边距，宽工具栏靠 10 个 flexGrow 按钮把误差摊薄了看不出来，
// 而 mosaic/eraser 只有 1 个按钮，误差全压在这个按钮和滑块上，看起来就像被压缩了。
void ToolSub::initSize(int btnCount, bool withColors, bool centerOnBtn)
{
	hasTools = true;
	this->centerOnBtn = centerOnBtn;
	auto count = btnCount + (withColors ? static_cast<int>(colors.size()) : 0);
	auto pxW = btnSize * count + sliderSize + sliderMargin * 2 + contentInsetX;
	setSize(pxW / dpi, getDesiredHeight() / dpi);
}

bool ToolSub::hasContent()
{
	return hasTools;
}

void ToolSub::hideTools()
{
	hasTools = false;
	if (!isVisible) return;
	hide();
	isVisible = false;
}

// ToolSub 永远紧贴 ToolMain 下方（三种模式都是），所以只跟着 ToolMain 走。
// x 默认与 ToolMain 左对齐；窄工具栏（mosaic/eraser）改为居中对齐到 ToolMain 上选中的那个按钮。
// 被屏幕边界裁剪后，同步修正箭头位置让它继续指向选中的按钮。
void ToolSub::updatePosition(const RECT& workArea)
{
	if (!hasTools) return;
	// btnCenterX 是选中按钮中心相对 ToolMain 左边的偏移
	auto btnCenterX = win->toolMain->getBtnCenterX();
	auto mainX = static_cast<float>(win->toolMain->x);
	auto px = centerOnBtn ? mainX + btnCenterX - w / 2.f : mainX;
	auto py = static_cast<float>(win->toolMain->y) + win->toolMain->h + mainGap;
	auto upperX = workArea.right - static_cast<int>(w);
	if (upperX < workArea.left) upperX = workArea.left;
	auto finalX = static_cast<int>(px);
	if (finalX < workArea.left) finalX = workArea.left;
	if (finalX > upperX) finalX = upperX;
	// 箭头始终指向按钮中心的屏幕位置，换算成窗口内坐标
	arrowX = mainX + btnCenterX - finalX;
	setPosition(finalX, static_cast<int>(py));
	if (isVisible) {
		refresh();
	}
	else {
		show();
		isVisible = true;
	}
}

