#include "pch.h"
#include <Windows.UI.Composition.Interop.h> 
#include "../Tool/ToolMain.h"
#include "../Tool/ToolSub.h"
#include "WinPin.h"
#include "WinCap.h"
#include "../App.h"

using namespace Microsoft::WRL;
namespace {
	std::vector<std::unique_ptr<WinPin>> winPins;

	int clampPos(float val, float size, int min, int max)
	{
		auto upper = max - static_cast<int>(size);
		if (upper < min) upper = min;
		auto result = static_cast<int>(val);
		if (result < min) result = min;
		if (result > upper) result = upper;
		return result;
	}
}

WinPin::WinPin(int x, int y, int w, int h) : Ling::WinBase()
{
	this->x = x;
	this->y = y;
	this->w = w;
	this->h = h;
    screenImg = WinCap::get()->getCutImg();
	toolMain = std::make_unique<ToolMain>(this);
    toolSub = std::make_unique<ToolSub>(this);
	layoutTools();
	onMoved.add([this]() { layoutTools(); });
}

// 把 ToolMain / ToolSub 摆到 WinPin 周围，始终靠 WinPin 右对齐，并尽量留在屏幕可视区内。
// 三种模式，按 ToolMain+ToolSub 的总高度决定（与 curId 是否为空无关，避免选中按钮时整组跳动）：
//   bottom : WinPin 下方，自上而下 ToolMain -> ToolSub
//   top    : 下方空间不足时改到 WinPin 上方，自上而下 ToolMain -> ToolSub -> WinPin
//   overlay: 上下都不足时覆盖在 WinPin 右下角，整组贴 WinPin 底边
// ToolSub 只在 curId 非空时显示，此时 ToolMain 上移为它腾出空间；ToolSub 永远紧贴 ToolMain 下方，
// 所以它那个朝上的小箭头在三种模式下都不需要翻转。
void WinPin::layoutTools()
{
	if (!toolMain || !toolSub) return;
	// WinPin 的 hwnd 此时可能还没创建（本函数会在构造期调用），所以用矩形而不是窗口句柄找显示器。
	RECT winRect{ x, y, x + static_cast<int>(w), y + static_cast<int>(h) };
	MONITORINFO mi{ .cbSize = sizeof(MONITORINFO) };
	auto monitor = MonitorFromRect(&winRect, MONITOR_DEFAULTTONEAREST);
	if (!monitor || !GetMonitorInfo(monitor, &mi)) {
		auto [sx, sy, sw, sh] = App::get()->getScreenArea();
		mi.rcWork = RECT{ sx, sy, sx + sw, sy + sh };
	}
	auto& wa = mi.rcWork;

	const auto gap = 5.f * dpi;              // 工具栏与 WinPin 之间的间距
	const auto subGap = ToolSub::mainGap;    // ToolMain 与 ToolSub 之间的间距
	const auto mainH = toolMain->h;
	const auto subH = toolSub->getDesiredHeight();
	const auto groupH = mainH + subGap + subH;
	const bool showSub = !toolMain->curId.empty() && toolSub->hasContent();
	// ToolSub 实际显示时 ToolMain 才上移让出它的位置（有些绘图按钮暂时还没有子工具栏）
	const auto usedH = showSub ? groupH : mainH;

	float mainY;
	if (winRect.bottom + gap + groupH <= wa.bottom) {         // bottom
		mainY = winRect.bottom + gap;
	}
	else if (winRect.top - gap - groupH >= wa.top) {          // top
		mainY = winRect.top - gap - usedH;
	}
	else {                                                    // overlay
		mainY = winRect.bottom - gap - usedH;
	}
	auto mainX = static_cast<float>(winRect.right) - toolMain->w;
	// 垂直方向按整组当前高度裁剪，避免 WinPin 超出工作区时把 ToolSub 挤到屏幕外
	toolMain->setPosition(clampPos(mainX, toolMain->w, wa.left, wa.right), clampPos(mainY, usedH, wa.top, wa.bottom));
	if (showSub) {
		toolSub->updatePosition(wa);
	}
	else {
		toolSub->hideTools();
	}
}

WinPin::~WinPin()
{
}

void WinPin::init(int x, int y, int w, int h)
{
	auto ptr = new WinPin(x,y,w,h);
	std::unique_ptr<WinPin> winPin{ ptr };
	ptr->createNativeWindow(WS_EX_TOPMOST| WS_EX_TOOLWINDOW, WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_POPUP);
	winPins.push_back(std::move(winPin));
}

void WinPin::onCreated()
{
    disableBorderRadius();
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
