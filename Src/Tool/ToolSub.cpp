#include "pch.h"
#include <Windows.UI.Composition.Interop.h> 
#include "../Win/WinPin.h"
#include "ToolSub.h"
#include "ToolMain.h"

using namespace Microsoft::WRL;

ToolSub::ToolSub(WinPin* win) :Ling::WinBase(), win(win)
{
	createNativeWindow(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, WS_POPUP);
}

ToolSub::~ToolSub()
{
}

void ToolSub::onCreated()
{
	auto d2d = Ling::D2D::get();
	// 画刷与设备（而非某次 BeginDraw 拿到的 context）绑定，建一次就够，paintBorder 每帧复用
	d2d->deviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), brushBg.GetAddressOf());
	d2d->deviceContext->CreateSolidColorBrush(D2D1::ColorF(0xA8A8A8), brushBorder.GetAddressOf());
	contentNode = body->makeChild<Ling::Node>();
	contentNode->setPositionType(Ling::Position::Absolute);
	// 四边让开描边宽度，上边再额外让开箭头区域（都是逻辑像素，setPosition 内部乘 dpi）
	contentNode->setPosition(Ling::Edge::Left, borderW);
	contentNode->setPosition(Ling::Edge::Top, marginTop + borderW);
	contentNode->setPosition(Ling::Edge::Right, borderW);
	contentNode->setPosition(Ling::Edge::Bottom, borderW);
	contentNode->setFlexDirection(Ling::FlexDirection::Row);
}

void ToolSub::showRectTools()
{
	contentNode->removeAllChildren();
	initSize(1, true);
	makeToggleBtn(L"\ue602", &isRectFill);
	sliderMin = 1.f;
	sliderMax = 16.f;
	sliderVal = 1.f;
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
	sliderMin = 1.f;
	sliderMax = 16.f;
	sliderVal = 3.f;
	initSlider();
	initColorBtns();
}

void ToolSub::showNumberTools()
{
	contentNode->removeAllChildren();
	initSize(1, true);
	makeToggleBtn(L"\ue605", &isNumberFill);
	sliderMin = 1.f;
	sliderMax = 36.f;
	sliderVal = 3.f;
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
	sliderMin = 18.f;
	sliderMax = 50.f;
	sliderVal = 20.f;
	initSize(1, false, true);
	makeToggleBtn(L"\ue602", &isMosaicRect);
	initSlider();
}

void ToolSub::showEraserTools()
{
	contentNode->removeAllChildren();
	sliderMin = 18.f;
	sliderMax = 50.f;
	sliderVal = 20.f;
	initSize(1, false, true);
	makeToggleBtn(L"\ue602", &isEraserRect);
	initSlider();
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
	// 几何体每帧都要按当前 w/h/arrowX 重建，用局部 ComPtr，别存成员：
	// 存成员时 GetAddressOf() 不会 Release 旧对象，等于每帧漏一个 ID2D1PathGeometry。
	ComPtr<ID2D1PathGeometry> borderPath;
	d2d->d2dFactory->CreatePathGeometry(borderPath.GetAddressOf());
	ComPtr<ID2D1GeometrySink> sink;
	borderPath->Open(sink.GetAddressOf());
	// 以下都是物理像素：D2D 直接画在 surface 上，不像 Ling 的 setter 会自己乘 dpi。
	// D2D 的描边以路径为中心线，所以路径放在距边 borderWPx/2 处，描边正好填满最外侧 borderWPx 像素。
	// 右/下边界要用 surface 的整数尺寸（窗口和 surface 都是 (int)w/(int)h），
	// 直接用带小数的 w/h 会让描边画到 surface 之外，看起来就是底边缺一条。
	auto borderWPx{ borderW * dpi };
	auto half{ borderWPx / 2.f };
	auto top{ toPx(marginTop) + half };
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
	ctx->FillGeometry(borderPath.Get(), brushBg.Get());
	ctx->DrawGeometry(borderPath.Get(), brushBorder.Get(), borderWPx);
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
	// contentNode->removeAllChildren() \u5df2\u7ecf\u628a\u4e0a\u4e00\u6279\u6309\u94ae\u9500\u6bc1\u4e86\uff0c\u8fd9\u91cc\u5fc5\u987b\u540c\u6b65\u6e05\u7a7a\uff0c
	// \u5426\u5219 colorBtns \u4f1a\u8d8a\u79ef\u8d8a\u957f\u4e14\u524d\u9762\u5168\u662f\u91ce\u6307\u9488\uff0cselectColorIndex \u4e5f\u4f1a\u8d8a\u754c\u3002
	colorBtns.clear();
	for (size_t i = 0; i < colors.size(); i++)
	{
		auto btn = contentNode->makeChild<Ling::Button>();
		btn->setHeight(btnSize-2.5);
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
		// \u91cd\u5efa\u540e\u8981\u628a\u5bf9\u52fe\u753b\u5728\u5f53\u524d\u9009\u4e2d\u7684\u90a3\u4e00\u9879\u4e0a\uff0c\u800c\u4e0d\u662f\u56fa\u5b9a\u7b2c\u4e00\u9879
		if (i == selectColorIndex) {
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
	btn->setHeight(btnSize - 2.5);
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
	slider->setWidth(sliderSize);
	slider->setMarginLeft(sliderMargin);
	slider->setMarginRight(sliderMargin);
	slider->setHeightPercent(100.f);
	// 值域与当前值都从字段来：切换工具时滑块会被销毁重建，靠 sliderVal 把用户调过的值带过来
	slider->setRange(sliderMin, sliderMax);
	slider->setValue(sliderVal);
	slider->setStep(1.f);
	slider->onValueChanged.add([this](Ling::Slider*, float val) { sliderVal = val; });
	slider->setThumbColor(0x888888FF);
	slider->setHoverThumbColor(0x888888FF);
	slider->setTrackColor(0x888888FF);
	slider->setFillColor(0x888888FF);
}

float ToolSub::toPx(float logical) const
{
	return std::floor(logical * dpi);
}

float ToolSub::getDesiredHeight()
{
	// 返回物理像素：调用方（WinPin::layoutTools）拿它和屏幕坐标、ToolMain->h 一起算，那些都是物理值。
	// 视觉高度（btnSize）与 ToolMain 的 h 保持一致 —— ToolMain 也是 h = btnSize，
	// 边框画在这个高度之内、不额外占空间；上面再加箭头区域。
	// 两段各自吸附到整数像素后再相加，避免和界内取整的差异。
	return toPx(btnSize) + toPx(marginTop);
}

// 宽度 = 工具按钮 + 颜色按钮 + 滑块（含左右 margin）。
// 之前这里漏算了滑块的真实宽度，宽工具栏靠 10 个 flexGrow 按钮把误差摊薄了看不出来，
// 而 mosaic/eraser 只有 1 个按钮，误差全压在这个按钮和滑块上，看起来就像被压缩了。
void ToolSub::initSize(int btnCount, bool withColors, bool centerOnBtn)
{
	hasTools = true;
	this->centerOnBtn = centerOnBtn;
	auto count = btnCount + (withColors ? static_cast<int>(colors.size()) : 0);
	// 宽度只按内容算，边框画在内容之内（与 ToolMain 一致，那边宽度也只累加按钮）。
	auto pxW = toPx(btnSize) * count + toPx(sliderSize) + toPx(sliderMargin) * 2;
	// setSize 收逻辑像素、内部再乘 dpi，所以这里把算好的物理宽高除回去
	setSize(pxW / dpi, getDesiredHeight() / dpi);
}

D2D1_COLOR_F ToolSub::getSelectedColor() const
{
	return Ling::Color(colors[selectColorIndex]).getD2DColor();
}

float ToolSub::getSliderVal() const
{
	return sliderVal*dpi;
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

