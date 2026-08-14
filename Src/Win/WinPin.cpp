#include "pch.h"
#include "../Tool/ToolMain.h"
#include "../Tool/ToolSub.h"
#include "../Shape/ShapeBase.h"
#include "../Shape/ShapeText.h"
#include "WinPin.h"
#include "WinCap.h"
#include "History.h"
#include "../App.h"
#include "../Util.h"

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

WinPin::WinPin(int x, int y, int w, int h, const std::vector<BYTE>* data) : Ling::WinBase(), history{ std::make_unique<History>(this) }
{
	this->x = x;
	this->y = y;
	this->w = w;
	this->h = h;
	if (data) {
		// 外部像素建底图。ShapeMosaic / ShapeEraser 会把它当画刷源，属性与 getCutImg() 出来的保持一致
		D2D1_BITMAP_PROPERTIES1 props{};
		props.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED);
		props.bitmapOptions = D2D1_BITMAP_OPTIONS_NONE;
		props.dpiX = 96.0f;
		props.dpiY = 96.0f;
		Ling::D2D::get()->deviceContext->CreateBitmap(D2D1::SizeU(w, h), data->data(), w * 4, &props, screenImg.GetAddressOf());
	}
	else {
		screenImg = WinCap::get()->getCutImg();
	}
	toolMain = std::make_unique<ToolMain>(this);
    toolSub = std::make_unique<ToolSub>(this);
	layoutTools();
	onMoved.add([this]() { layoutTools(); });
	// DPI 变了（用户改了缩放比例，或者窗口被拖到缩放比例不同的显示器上）：系统会按新旧缩放比
	// 把窗口整体放大一圈，但贴图窗口的尺寸是钉死在底图像素上的 —— 底图按原始像素画，
	// shape 的坐标也都是相对底图的物理像素，跟着缩放只会让窗口比图大一圈：
	// 右边、下边多出一条空白，边框看着比左上两边粗（描边居中，左上那半截被窗口边裁掉了），
	// 工具条也会按虚高的宽高往右下偏。所以等系统把建议矩形应用完（紧随而来的 WM_SIZE）
	// 再把尺寸掰回底图大小，并重排工具条。位置不能在 onDpiChanged 里改 ——
	// 那个事件在系统建议矩形生效之前触发，改了马上被覆盖
	onDpiChanged.add([this]() { dpiChanged = true; });
	onSizeChanged.add([this]() {
		if (!dpiChanged) return;
		dpiChanged = false;
		applyWinSize();
		layoutTools();
	});
	onMouseDown.add([this](POINT pos, BOOL isRight) {this->onDown(pos, isRight);});
	onMouseMove.add([this](POINT pos) {this->onMove(pos);});
	onMouseUp.add([this](POINT pos, BOOL isRight) {this->onUp(pos, isRight);});
	// Ling 传进来的是已经换算成滚动距离的 space（一格 = 60 逻辑像素 × dpi），
	// shape 只关心方向，这里按符号还原成 ±WHEEL_DELTA
	onMouseWheel.add([this](POINT pos, float space) {
		// Ling 的滚轮事件不带修饰键状态，自己查：按住 Ctrl 是缩放窗口，不是调 shape
		if (GetKeyState(VK_CONTROL) & 0x8000) {
			// 一格 10%，按当前倍数等比走，放大和缩小的手感才对称
			applyScale(scale * (space > 0 ? 1.1f : 1.f / 1.1f), pos);
			return;
		}
		if (!shapeHover) return;
		auto imgPos = toImgPos(pos);
		shapeHover->mouseWheel((float)imgPos.x, (float)imgPos.y, space > 0 ? (short)WHEEL_DELTA : (short)-WHEEL_DELTA);
	});
	onTimer.add([this](UINT id) {this->onTimerCB(id);});
	onKeyDown.add([this](UINT key) {this->onKey(key);});
	onDestroy.add([this]() { this->onClosed(); });
}

// WinBase::close() 里 DestroyWindow 之后同步触发 onDestroy，所以这个函数很可能是从
// ToolMain 的按钮回调里一路调进来的（点了 close 按钮）。此时 ToolMain::onClick 还在栈上，
// 而 toolMain 是 WinPin 的成员 —— 在这里直接把自己从 winPins 里擦掉就是 use-after-free。
// 因此：窗口句柄立即销毁（用户马上看到界面消失），C++ 对象的释放推迟到下一轮消息循环。
void WinPin::onClosed()
{
	// 防止 close() 被走两遍（比如按钮和快捷键先后触发）时排两次销毁
	if (isClosed) return;
	isClosed = true;
	// 先收起附属窗口，再让出 hover 指针 —— shapeHover 指向 history 里的元素，
	// history 随 WinPin 一起析构，留着悬空指针没意义
	if (toolSub) toolSub->close();
	if (toolMain) toolMain->close();
	shapeHover = nullptr;
	editingText = nullptr;
	// screenImg / canvas / history 都是成员（canvas 挂在 body 的子节点上），随下面这次 erase 一并释放
	Ling::App::get()->dq.TryEnqueue([this]() {
		std::erase_if(winPins, [this](const std::unique_ptr<WinPin>& p) { return p.get() == this; });
		// 用完即走模式下，最后一个贴图窗口关掉就退出进程，不驻留在系统里。
		// 贴图可以同时开好几个（标注、长截图各来一张），所以得等它们都没了才退
		if (winPins.empty()) {
			if (Ling::App::get()->args[L"--auto-quit"] == L"true") {
				Ling::App::get()->quit(0);
			}
			else {
				App::trimMemoryLater(); //只剩托盘图标了，把显卡那边的缓存还回去
			}
		}
	});
}

bool WinPin::hasWindow()
{
	return !winPins.empty();
}

D2D1_SIZE_U WinPin::getImgSize() const
{
	if (!screenImg) return D2D1::SizeU(0, 0);
	// 要的是像素数，所以问 GetPixelSize 而不是 GetSize（后者返回的是按位图自身 dpi 折算的 DIP）
	return screenImg->GetPixelSize();
}

void WinPin::applyWinSize()
{
	auto sz = getImgSize();
	if (!hwnd || sz.width == 0 || sz.height == 0) return;
	auto newW = std::max(1, static_cast<int>(std::lround(sz.width * scale)));
	auto newH = std::max(1, static_cast<int>(std::lround(sz.height * scale)));
	w = static_cast<float>(newW);
	h = static_cast<float>(newH);
	// 不走 setSize：它收的是逻辑像素、内部还要乘一遍 dpi，而这里的宽高本来就是物理像素
	SetWindowPos(hwnd, nullptr, 0, 0, newW, newH, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREDRAW);
}

POINT WinPin::toImgPos(const POINT& pos) const
{
	if (scale == 1.f) return pos;
	return POINT{ static_cast<LONG>(std::lround(pos.x / scale)), static_cast<LONG>(std::lround(pos.y / scale)) };
}

void WinPin::applyScale(float newScale, POINT anchor)
{
	auto sz = getImgSize();
	if (sz.width == 0 || sz.height == 0) return;
	// 上限跟着底图大小走：窗口边长再大，swap chain 那块显存也吃不消，人也看不过来；
	// 但至少要能回到 1 倍，所以下面用 max 兜一下
	auto maxScale = std::max(1.f, std::min(8.f, 16000.f / std::max(sz.width, sz.height)));
	newScale = std::clamp(newScale, 0.1f, maxScale);
	if (std::abs(newScale - scale) < 0.0001f) return;
	// 编辑中的文字是 TextBox（真控件）画的，缩放期间它的位置、字号都得跟着重算，
	// 与其在缩放过程里一路同步，不如先收尾把文字交回 ShapeText 自己画 —— 之后它就跟着一起缩了
	if (editingText) editingText->finishEdit();
	// anchor 底下那个底图上的点，缩放前后都要停在光标下：屏幕坐标 = 窗口原点 + 底图点 × 倍数
	auto imgX = anchor.x / scale;
	auto imgY = anchor.y / scale;
	scale = newScale;
	applyWinSize();
	// setPosition 内部会 SetWindowPos，随后的 WM_MOVE 会带出 onMoved -> layoutTools
	setPosition(x + anchor.x - static_cast<int>(std::lround(imgX * scale)),
		y + anchor.y - static_cast<int>(std::lround(imgY * scale)));
	makeScaleTip();
	// 停手 800 毫秒后由定时器把倍数提示收掉。同一个 id 再调一次 SetTimer 就是重新计时，
	// 所以连续滚动期间它一直不会触发
	setTimer(800, 101);
	layoutTools();
	refresh();
}

void WinPin::makeScaleTip()
{
	scaleTip = Util::makeTextLayout(std::format(L"{}%", static_cast<int>(std::lround(scale * 100.f))),
		FLT_MAX, FLT_MAX, 11.f * dpi);
}

// 画在窗口右上角，半透明底 + 白字，与 CutMask 上那个坐标标签一个路子。
// 调用方要先把缩放变换收回去：这是窗口装饰，不跟着图一起放大
void WinPin::paintScaleTip(ID2D1DeviceContext* ctx)
{
	if (!scaleTip || !brushTipBg) return;
	DWRITE_TEXT_METRICS tm{};
	if (FAILED(scaleTip->GetMetrics(&tm))) return;
	auto pad = 3.f * dpi;
	auto margin = 5.f * dpi;
	D2D1_RECT_F bgRect{ w - margin - tm.width - pad * 2, margin, w - margin, margin + tm.height + pad * 2 };
	// 图小到装不下提示时，贴着左边画，别画到窗口外面去
	if (bgRect.left < margin) bgRect.left = margin;
	ctx->FillRectangle(bgRect, brushTipBg.Get());
	ctx->DrawTextLayout({ bgRect.left + pad, bgRect.top + pad }, scaleTip.Get(), brushTipText.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
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

void WinPin::initFromData(int x, int y, int w, int h, std::vector<BYTE>& data)
{
	auto ptr = new WinPin(x, y, w, h, &data);
	std::unique_ptr<WinPin> winPin{ ptr };
	ptr->createNativeWindow(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_POPUP);
	winPins.push_back(std::move(winPin));
}

void WinPin::onCreated()
{
    disableBorderRadius();
    auto d2d = Ling::D2D::get();
    // 画布铺满窗口，走 swap chain（双缓冲）后端，避免拖动 shape 时整帧闪烁
    canvas = body->makeChild<Ling::Canvas>();
    canvas->enableSwapChain();
    canvas->setSizePercent(100.f, 100.f);
    d2d->deviceContext->CreateSolidColorBrush(D2D1::ColorF(0x1677ff), borderBrush.GetAddressOf());
    d2d->deviceContext->CreateSolidColorBrush(D2D1::ColorF(0x000000, 0.46f), brushTipBg.GetAddressOf());
    d2d->deviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), brushTipText.GetAddressOf());
    show();
}

void WinPin::layout()
{
    Ling::WinBase::layout();
    if (!screenImg || !canvas) return;
    auto ctx = canvas->startPaint();
    if (!ctx) return;
    ctx->Clear(0);
    auto sz = screenImg->GetSize();
    D2D1_RECT_F destRect = D2D1::RectF(0, 0, sz.width, sz.height);
    // 底图和 shape 都是按底图像素画的，放大缩小整个交给这个变换，
    // 笔宽、夹点跟着一起缩 —— 鼠标坐标进来时也除掉了倍数，所以命中判定天然对得上
    ctx->SetTransform(D2D1::Matrix3x2F::Scale(scale, scale));
    ctx->DrawBitmap(screenImg.Get(), destRect);
	for (auto& shape : history->shapes)
	{
		if (!shape->isUndo) {
			shape->paint(ctx);
		}
	}
	if (!isMouseDown && shapeHover) {
		shapeHover->paintDragger(ctx);
	}
	// 蓝边框和倍数提示属于窗口装饰，不跟着图缩放：变换收回来，按窗口坐标画。
	// 边框也因此从"底图矩形"改成"窗口矩形"，任何倍数下都是 2*dpi 粗
	ctx->SetTransform(D2D1::Matrix3x2F::Identity());
	ctx->DrawRectangle(D2D1::RectF(0.f, 0.f, w, h), borderBrush.Get(), 2*dpi);
	paintScaleTip(ctx);
    canvas->finishPaint();
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
	// Ctrl+滚轮放大后窗口可以比屏幕大（超出的部分自然被裁掉）。默认的最大跟踪尺寸只有
	// 主显示器那么大，不放开的话 SetWindowPos 会被系统按住，放大就到此为止了
	mmi->ptMaxTrackSize.x = 20000;
	mmi->ptMaxTrackSize.y = 20000;
}

void WinPin::onDown(POINT pos, BOOL isRight)
{
	// 编辑文本时，落在文本框里的点击整个交给 TextBox（它自己订阅了窗口的鼠标事件）。
	// 这里不能抢先 SetCapture / 置 isMouseDown，否则拖选文本会被当成拖 shape。
	if (editingText && textBox && textBox->isPosIn(pos)) return;
	// 右键在贴图窗口上没有任何动作
	if (isRight) return;
	// 双击判定得自己做，做法同 WinCap::onDown：Ling 的窗口类没带 CS_DBLCLKS，
	// WM_LBUTTONDBLCLK 根本不会来，只能拿系统的双击间隔和双击判定框自己认。
	// 与 WinCap 唯一的不同是这里比屏幕坐标而不是客户区坐标：拖动贴图窗口时窗口跟着光标走，
	// 抓住的那一点始终停在光标下，光标的客户区坐标几乎不变 —— 用客户区坐标会把
	// "拖一下松手再拖一下"当成双击，图和标注就这么被复制走关掉了。
	// 而拖动必然意味着光标在屏幕上真的移动过，屏幕坐标能把这种情况分开
	POINT screenPos{};
	GetCursorPos(&screenPos);
	auto now = GetTickCount64();
	bool isDblClick = (now - lastDownTime <= GetDoubleClickTime())
		&& std::abs(screenPos.x - lastDownPos.x) <= GetSystemMetrics(SM_CXDOUBLECLK)
		&& std::abs(screenPos.y - lastDownPos.y) <= GetSystemMetrics(SM_CYDOUBLECLK);
	lastDownTime = now;
	lastDownPos = screenPos;
	// 双击 = Ctrl+C：把图连标注一起送进剪切板并关窗，选着画笔也一样（等价于按 Ctrl+C，
	// 手里拿着什么工具都不该影响这个手势）。要在下面所有分支之前处理：
	// 这一下既不是画画也不是拖窗，不该留下 capture、更不该新建 shape。
	// 编辑文字时不算：双击归文本框（选中单词），点在框外才会走到这里
	if (isDblClick && !editingText) {
		// 前半段那一下点击是这个手势的一部分，它顺手放下的元素（只有序号是按一下就成形的，
		// 别的都在抬手时按"没画出东西"清掉了）不该被带进剪切板
		if (prevPressCreatedShape) history->undo();
		copyToClipboard();
		return;
	}
	// 记的是按下点在窗口内的偏移（客户区坐标），拖动时用它把抓住的那一点保持在光标下
	pressPos.x = pos.x;
	pressPos.y = pos.y;
	isMouseDown = true;
	hasDragged = false;
	SetCapture(hwnd);
	if (toolMain->curId == L"") { //没选画笔，左键是拖窗口，拖的时候把工具条收起来
		toolMain->hide();
		return;
	}
	// 以下都是交给 shape 的坐标，一律换算成底图像素（拖窗口那条路仍用窗口坐标）
	auto imgPos = toImgPos(pos);
	if (shapeHover) {
		newShape = nullptr; //改的是已有元素，不参与空元素判定
		shapeHover->mouseDown((float)imgPos.x, (float)imgPos.y);
		return;
	}
	shapeHover = history->createShape(toolMain->curId, imgPos.x, imgPos.y);
	newShape = shapeHover;
}

void WinPin::onMove(POINT pos)
{
	// 同 onDown：文本框里的移动归 TextBox（拖选、滚动条 hover），不参与 shape 的 hover 判定
	if (editingText && textBox && textBox->isPosIn(pos)) return;
	// 拖窗口用的是窗口坐标（pressPos 也是），只有交给 shape 的才换算成底图像素
	auto imgPos = toImgPos(pos);
	if (isMouseDown) {
		if (toolMain->curId == L"") {
			setPosition(x + pos.x - pressPos.x, y + pos.y - pressPos.y);
			return;
		}
		else if(shapeHover) {
			// 光标一步没挪也会来 WM_MOUSEMOVE，所以跟按下点比一下再算拖动
			if (pos.x != pressPos.x || pos.y != pressPos.y) hasDragged = true;
			shapeHover->mouseDrag((float)imgPos.x, (float)imgPos.y);
			refresh();
		}
	}
	else
	{
		if (toolMain->curId == L"") return;
		int i{ (int)(history->shapes.size() - 1) };
		for (; i >= 0; i--)
		{
			auto cur = history->shapes[i].get();
			if (cur->isUndo) continue;
			cur->mouseMove((float)imgPos.x, (float)imgPos.y);
			if (cur->hoverDraggerIndex >= 0) {
				if (shapeHover != cur) {
					shapeHover = cur;
					setTimer(800, 100);
					refresh();
				}
				return;
			}
		}
		if (shapeHover) {
			shapeHover = nullptr;
		}
	}
}

void WinPin::onUp(POINT pos, BOOL isRight)
{
	isMouseDown = false;
	ReleaseCapture();
	auto justCreated = newShape;
	newShape = nullptr;
	// 这一下按下有没有新建出一个留得住的元素：紧接着来第二下凑成双击时要把它撤掉（见 onDown）
	prevPressCreatedShape = false;
	if (toolMain->curId == L"") { //state为空时，是在拖动窗口
		layoutTools();
		toolMain->show();
	}
	else if (shapeHover) {
		// 新建的这一笔按下马上弹起，什么也没画出来：直接丢掉，
		// 也省了 mouseUp 里的收尾开销（马赛克那边要把 GPU 像素读回内存，不该为一个要删的元素白做）
		if (shapeHover == justCreated && !hasDragged && !shapeHover->isValidWithoutDrag()) {
			history->removeShape(shapeHover); //它会顺手清掉 shapeHover 并刷新
			return;
		}
		prevPressCreatedShape = (shapeHover == justCreated);
		auto imgPos = toImgPos(pos);
		shapeHover->mouseUp((float)imgPos.x, (float)imgPos.y);
		refresh();
		setTimer(800, 100);
	}

}

void WinPin::onTimerCB(UINT id)
{
	if (id == 101) { //缩放停手了，收掉右上角的倍数提示
		killTimer(101);
		scaleTip = nullptr;
		refresh();
		return;
	}
	if (id != 100) return;
	if (!shapeHover) {
		refresh();
		killTimer(100);
	}
}

Ling::TextBox* WinPin::getTextBox()
{
	if (textBox) return textBox;
	// 建在 canvas 之后：Composition 的子 visual 按插入顺序叠放，文本框要盖在截图上面。
	// 绝对定位，位置由 ShapeText 按自己的矩形指定，不参与 body 的 flex 排布。
	textBox = body->makeChild<Ling::TextBox>();
	textBox->setPositionType(Ling::Position::Absolute);
	// 不折行、尺寸跟着文字长，与 2.4.25 的文本窗口一致
	textBox->setAutoSize(true);
	// 背景、边框都不画：编辑中看到的就是最终效果，那圈虚线框由 ShapeText 自己画
	textBox->hide();
	return textBox;
}

void WinPin::setEditingText(ShapeText* shape)
{
	editingText = shape;
}

void WinPin::onToolStyleChanged()
{
	if (editingText) editingText->applyStyle();
}

void WinPin::onKey(UINT key)
{
	// 编辑文本时所有按键都归 TextBox：否则 Ctrl+C 复制的是截图、回车会保存并关窗、
	// Delete 删掉的是整个 shape、ESC 直接把窗口关了。ESC 结束编辑由 TextBox 自己处理。
	if (editingText) return;
	bool ctrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
	if (ctrl && key == 'Z') {
		history->undo();
	}
	else if (ctrl && key == 'Y') {
		history->redo();
	}
	else if (ctrl && key == 'C') {
		copyToClipboard();
	}
	else if (ctrl && key == 'S') {
		saveToFile();
	}
	else if (key == VK_RETURN) {
		copyToClipboard();
	}
	else if (key == VK_DELETE) {
		history->removeHoverShape();
	}
	else if (key == VK_ESCAPE) {
		close();
	}
}

void WinPin::copyToClipboard()
{
	std::vector<BYTE> pixels;
	if (!getImagePixels(pixels)) return;
	Util::saveToClipboard((int)w, (int)h, pixels.data());
	close();
}

void WinPin::saveToFile()
{
	auto foregroundBeforeDialog = GetForegroundWindow();
	auto path = Util::getSaveFilePath(hwnd);
	if (path.empty()) {   // 用户取消
		restoreWindowState(foregroundBeforeDialog);
		return;
	}
	std::vector<BYTE> pixels;
	if (!getImagePixels(pixels)) {
		restoreWindowState(foregroundBeforeDialog);
		return;
	}
	if (Util::saveToFile(path, (int)w, (int)h, pixels.data())) {
		close();
	}
	else {
		restoreWindowState(foregroundBeforeDialog);
	}
}

// 另存为对话框关掉后会把 owner(hwnd) 变成活动窗口，WinPin 一被激活就会盖住 ToolMain。
// 这里把三个窗口重新压到 topmost，并把前台还给开对话框之前的那个窗口。
void WinPin::restoreWindowState(HWND foregroundBeforeDialog)
{
	SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
	if (toolMain && toolMain->hwnd && IsWindowVisible(toolMain->hwnd)) {
		SetWindowPos(toolMain->hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
	}
	if (toolSub && toolSub->hwnd && IsWindowVisible(toolSub->hwnd)) {
		SetWindowPos(toolSub->hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
	}
	if (foregroundBeforeDialog && foregroundBeforeDialog != hwnd && IsWindowVisible(foregroundBeforeDialog)) {
		SetForegroundWindow(foregroundBeforeDialog);
	}
}

// 离屏把底图和 shape 合成到一张新位图上再读回像素。
// 不直接画到 screenImg 上：它是 ShapeEraser 的"原样"来源，也是 ShapeMosaic 的取样来源，
// 一旦被 shape 覆写，之后再擦除/打码就会拿到已经画过的画面。
// 用 d2d->deviceContext 做离屏是安全的，SetTarget → BeginDraw → EndDraw → SetTarget(nullptr) 在本函数内闭环。
bool WinPin::getImagePixels(std::vector<BYTE>& pixels)
{
	// 尺寸一律取底图的像素尺寸，不用窗口的 w/h —— Ctrl+滚轮缩放改的是窗口，
	// 导出的图该始终是原始大小
	auto imgSize = getImgSize();
	if (imgSize.width == 0 || imgSize.height == 0) return false;
	// 编辑中的文字是 TextBox 自己那层画的，进不了下面这个离屏 target。
	// 先收尾，把文字交回 ShapeText 自己画，保存/复制出去的图才有它。
	if (editingText) editingText->finishEdit();
	auto size = imgSize;
	auto d2d = Ling::D2D::get();
	auto ctx = d2d->deviceContext.Get();

	D2D1_BITMAP_PROPERTIES1 targetProps{
		.pixelFormat{ D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED) },
		.dpiX{ 96.0f }, .dpiY{ 96.0f },
		.bitmapOptions{ D2D1_BITMAP_OPTIONS_TARGET }
	};
	ComPtr<ID2D1Bitmap1> targetBmp;
	auto hr = ctx->CreateBitmap(size, nullptr, 0, &targetProps, targetBmp.GetAddressOf());
	if (FAILED(hr)) return false;

	ctx->SetTarget(targetBmp.Get());
	ctx->SetTransform(D2D1::Matrix3x2F::Identity());
	ctx->BeginDraw();
	ctx->Clear(D2D1::ColorF(0, 0.0f));
	ctx->DrawBitmap(screenImg.Get(), D2D1::RectF(0.f, 0.f, (float)imgSize.width, (float)imgSize.height));
	for (auto& shape : history->shapes)
	{
		if (!shape->isUndo) {
			shape->paint(ctx);
		}
	}
	hr = ctx->EndDraw();
	// 解绑，下面 CopyFromBitmap 才能把它当 source 读
	ctx->SetTarget(nullptr);
	if (FAILED(hr)) return false;

	// GPU 上的 target 位图不能直接 Map，得先拷到一块带 CPU_READ 的位图上
	D2D1_BITMAP_PROPERTIES1 cpuProps{
		.pixelFormat{ targetBmp->GetPixelFormat() },
		.dpiX{ 96.0f }, .dpiY{ 96.0f },
		.bitmapOptions{ D2D1_BITMAP_OPTIONS_CPU_READ | D2D1_BITMAP_OPTIONS_CANNOT_DRAW }
	};
	ComPtr<ID2D1Bitmap1> cpuBmp;
	hr = ctx->CreateBitmap(size, nullptr, 0, &cpuProps, cpuBmp.GetAddressOf());
	if (FAILED(hr)) return false;
	hr = cpuBmp->CopyFromBitmap(nullptr, targetBmp.Get(), nullptr);
	if (FAILED(hr)) return false;
	D2D1_MAPPED_RECT mapped{};
	hr = cpuBmp->Map(D2D1_MAP_OPTIONS_READ, &mapped);
	if (FAILED(hr)) return false;
	// mapped.pitch 按 GPU 行对齐，可能大于 w*4；剪切板和 WIC 都要求紧凑步长，逐行紧缩
	const UINT32 rowBytes = size.width * 4;
	pixels.resize((size_t)rowBytes * size.height);
	for (UINT32 row = 0; row < size.height; ++row)
	{
		CopyMemory(pixels.data() + (size_t)row * rowBytes,
			mapped.bits + (size_t)row * mapped.pitch,
			rowBytes);
	}
	cpuBmp->Unmap();
	return true;
}

BOOL WinPin::setCursor()
{
	// 编辑文本时光标形状交给 TextBox 决定（文本区 I 形、滚动条箭头）。
	// 本函数覆写了基类且不调用它，TextBox 挂在 onCursor 上的那个订阅不会自己被触发，得手动发一次。
	if (editingText) {
		bool handled{ false };
		onCursor(&handled);
		if (handled) return TRUE;
	}
	if (toolMain->curId == L"") {
		SetCursor(LoadCursor(nullptr, IDC_SIZEALL));
		return TRUE;
	}
	if (shapeHover) {
		shapeHover->setCursor();
		return TRUE;
	}
	if (toolMain->curId == L"text") {
		SetCursor(LoadCursor(nullptr, IDC_IBEAM));
	}
	else {
		SetCursor(LoadCursor(nullptr, IDC_CROSS));
	}
	return TRUE;
}
