#include "pch.h"
#include <Windows.UI.Composition.Interop.h> 
#include "../Win/WinPin.h"
#include "ToolSub.h"
#include "ToolMain.h"

using namespace Microsoft::WRL;

ToolSub::ToolSub(WinPin* win) :Ling::WinBase(), win(win)
{
	btnSize = std::floor(btnSize * dpi);
	sliderSize *= dpi;
	sliderMargin *= dpi;
	// 描边宽度与 ToolMain 的 setBorder(1.f) 保持一致：那边最终是 borderWidth*dpi，不取整。
	// 取整会让高 DPI 下 ToolSub 的边框比 ToolMain 细（1.5 被压成 1）。
	borderW *= dpi;
	// 箭头区域取整到整数物理像素，避免窗口高度出现小数被 SetWindowPos(int) 截断
	marginTop = std::floor(marginTop * dpi);
	createNativeWindow(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, WS_POPUP);
}

ToolSub::~ToolSub()
{
}

void ToolSub::showRectTools()
{
	contentNode->removeAllChildren();
	initSize(1, true);
	makeToggleBtn(L"\ue602", &isRectFill);
	initSlider();
	initColorBtns();
}

void ToolSub::showEllipseTools()
{
	contentNode->removeAllChildren();
	initSize(1, true);
	makeToggleBtn(L"\ue600", &isEllipseFill);
	initSlider();
	initColorBtns();
}

void ToolSub::showArrowTools()
{
	contentNode->removeAllChildren();
	initSize(1, true);
	makeToggleBtn(L"\ue604", &isArrowFill);
	initSlider();
	initColorBtns();
}

void ToolSub::showNumberTools()
{
	contentNode->removeAllChildren();
	initSize(1, true);
	makeToggleBtn(L"\ue605", &isNumberFill);
	initSlider();
	initColorBtns();
}

void ToolSub::showLineTools()
{
	contentNode->removeAllChildren();
	initSize(1, true);
	makeToggleBtn(L"\ue607", &isLineTransparent);
	initSlider();
	initColorBtns();
}

void ToolSub::showTextTools()
{
	contentNode->removeAllChildren();
	initSize(2, true);
	makeToggleBtn(L"\ue634", &isTextBold);
	makeToggleBtn(L"\ue682", &isTextItalic);
	initSlider();
	initColorBtns();
}

void ToolSub::showMosaicTools()
{
	contentNode->removeAllChildren();
	initSize(1, false, true);
	makeToggleBtn(L"\ue602", &isMosaicRect);
	initSlider();
}

void ToolSub::showEraserTools()
{
	contentNode->removeAllChildren();
	initSize(1, false, true);
	makeToggleBtn(L"\ue602", &isEraserRect);
	initSlider();
}

void ToolSub::onCreated()
{
	auto d2d = Ling::D2D::get();
	contentNode = body->makeChild<Ling::Node>();
	contentNode->setPositionType(Ling::Position::Absolute);
	// 四边让开描边宽度，上边再额外让开箭头区域。
	// borderW/marginTop 是物理像素，setPosition 内部会乘 dpi，所以这里要先除回去。
	contentNode->setPosition(Ling::Edge::Left, borderW / dpi);
	contentNode->setPosition(Ling::Edge::Top, (marginTop + borderW) / dpi);
	contentNode->setPosition(Ling::Edge::Right, borderW / dpi);
	contentNode->setPosition(Ling::Edge::Bottom, borderW / dpi);
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
	// D2D 的描边以路径为中心线，所以路径放在距边 borderW/2 处，描边正好填满最外侧 borderW 像素。
	// 右/下边界要用 surface 的整数尺寸（窗口和 surface 都是 (int)w/(int)h），
	// 直接用带小数的 w/h 会让描边画到 surface 之外，看起来就是底边缺一条。
	auto half{ borderW / 2.f };
	auto top{ marginTop + half };
	auto right{ std::floor(w) - half };
	auto bottom{ std::floor(h) - half };
	sink->BeginFigure({ half,top }, D2D1_FIGURE_BEGIN_FILLED);
	D2D1_POINT_2F points[6] = {
		D2D1_POINT_2F{arrowX - 3.f * dpi,top},
		D2D1_POINT_2F{arrowX,half},
		D2D1_POINT_2F{arrowX + 3.f * dpi,top},
		D2D1_POINT_2F{right,top},
		D2D1_POINT_2F{right,bottom},
		D2D1_POINT_2F{half,bottom}
	};
	sink->AddLines(points, 6);
	sink->EndFigure(D2D1_FIGURE_END_CLOSED);
	sink->Close();
	ComPtr<ID2D1SolidColorBrush> brushBg;
	ComPtr<ID2D1SolidColorBrush> brushBorder;
	ctx->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), brushBg.GetAddressOf());
	ctx->CreateSolidColorBrush(D2D1::ColorF(0xA8A8A8), brushBorder.GetAddressOf());
	ctx->FillGeometry(borderPath.Get(), brushBg.Get());
	ctx->DrawGeometry(borderPath.Get(), brushBorder.Get(), borderW);
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

void ToolSub::applyToggleStyle(Ling::Button* btn, bool selected)
{
	if (selected) {
		// 选中态 hover 色与常态一致，避免鼠标移上去时选中效果被 hover 覆盖掉
		btn->setBg(0xe6f4ffff);
		btn->setHoverBg(0xe6f4ffff);
	}
	else {
		btn->setBg(0);
		btn->setHoverBg(0xF2F2F2ff);
	}
}

Ling::Button* ToolSub::makeToggleBtn(const std::wstring& text, bool* flag)
{
	auto btn = contentNode->makeChild<Ling::Button>();
	btn->setText(text);
	btn->setHeightPercent(100.f);
	btn->setFlexGrow(1.f);
	btn->setFontFamily(L"icon");
	btn->setFontSize(13.f);
	applyToggleStyle(btn, *flag);
	// flag 指向 ToolSub 的成员，生命周期与 this 相同，btn 也挂在 this 的节点树上，捕获裸指针安全
	btn->onClick.add([this, flag](Ling::Button* b) {
		*flag = !*flag;
		applyToggleStyle(b, *flag);
	});
	return btn;
}

void ToolSub::initSlider()
{
	auto slider = contentNode->makeChild<Ling::Slider>();
	// 尺寸从字段来，别写字面量：initSize 按同样的字段算窗口宽度，
	// 两边各写一份的话改了一处就会宽度不匹配（flex 会把误差压在按钮和滑块上）。
	// 字段在构造函数里已经乘过 dpi，而 setWidth/setMargin 内部还会再乘一次，所以这里要除回去。
	slider->setWidth(sliderSize / dpi);
	slider->setMarginLeft(sliderMargin / dpi);
	slider->setMarginRight(sliderMargin / dpi);
	slider->setHeightPercent(100.f);
	slider->setThumbColor(0x888888FF);
	slider->setHoverThumbColor(0x888888FF);
	slider->setTrackColor(0x888888FF);
	slider->setFillColor(0x888888FF);
}

float ToolSub::getDesiredHeight()
{
	// 视觉高度（btnSize）与 ToolMain 的 h 保持一致 —— ToolMain 也是 h = btnSize，
	// 边框画在这个高度之内、不额外占空间；上面再加箭头区域。
	// 整体向上取整：窗口尺寸最终过 SetWindowPos(int)，留小数会被截断，
	// 而 Yoga/paintBorder 仍按小数布局，底边就会错开半像素、压住边框。
	return std::ceil(btnSize + marginTop);
}

// 宽度 = 工具按钮 + 颜色按钮 + 滑块（含左右 margin）+ contentNode 左右内边距。
// 之前这里漏算了滑块的真实宽度和内边距，宽工具栏靠 10 个 flexGrow 按钮把误差摊薄了看不出来，
// 而 mosaic/eraser 只有 1 个按钮，误差全压在这个按钮和滑块上，看起来就像被压缩了。
void ToolSub::initSize(int btnCount, bool withColors, bool centerOnBtn)
{
	hasTools = true;
	this->centerOnBtn = centerOnBtn;
	auto count = btnCount + (withColors ? static_cast<int>(colors.size()) : 0);
	// 宽度只按内容算，边框画在内容之内（与 ToolMain 一致，那边宽度也只累加按钮）。
	// 向上取整避免 SetWindowPos(int) 截断后布局与实际窗口差半像素。
	auto pxW = std::ceil(btnSize * count + sliderSize + sliderMargin * 2);
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

