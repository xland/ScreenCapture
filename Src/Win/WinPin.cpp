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
	onMouseDown.add([this](POINT pos, BOOL isRight) {this->onDown(pos, isRight);});
	onMouseMove.add([this](POINT pos) {this->onMove(pos);});
	onMouseUp.add([this](POINT pos, BOOL isRight) {this->onUp(pos, isRight);});
	// Ling 传进来的是已经换算成滚动距离的 space（一格 = 60 逻辑像素 × dpi），
	// shape 只关心方向，这里按符号还原成 ±WHEEL_DELTA
	onMouseWheel.add([this](POINT pos, float space) {
		if (!shapeHover) return;
		shapeHover->mouseWheel((float)pos.x, (float)pos.y, space > 0 ? (short)WHEEL_DELTA : (short)-WHEEL_DELTA);
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
	});
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
	ctx->DrawRectangle(destRect, borderBrush.Get(), 2*dpi);
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
}

void WinPin::onDown(POINT pos, BOOL isRight)
{
	// 编辑文本时，落在文本框里的点击整个交给 TextBox（它自己订阅了窗口的鼠标事件）。
	// 这里不能抢先 SetCapture / 置 isMouseDown，否则拖选文本会被当成拖 shape。
	if (editingText && textBox && textBox->isPosIn(pos)) return;
	if (isRight) {
		//右键：取消置顶并隐藏工具条；再次左键按下时恢复
		if (isTopmost) {
			SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
			LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
			exStyle &= ~WS_EX_TOOLWINDOW;
			exStyle |= WS_EX_APPWINDOW;
			SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle);
			toolMain->cancelSelect();
			toolMain->hide();
			isTopmost = false;
		}
		return;
	}
	// 记的是按下点在窗口内的偏移（客户区坐标），拖动时用它把抓住的那一点保持在光标下
	pressPos.x = pos.x;
	pressPos.y = pos.y;
	isMouseDown = true;
	SetCapture(hwnd);
	if (!isTopmost) {
		//左键：从"未置顶"状态恢复。先重新置顶，并复位工具条状态；
		//ToolMain 的显示交给 onMouseUp 走"拖窗结束"的通用路径，避免拖拽时工具条不跟随
		SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
		LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
		exStyle &= ~WS_EX_APPWINDOW;
		exStyle |= WS_EX_TOOLWINDOW;
		SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle);
		toolMain->cancelSelect();
		isTopmost = true;
		return;
	}
	if (toolMain->curId == L"") {
		toolMain->hide();
		return;
	}
	if (shapeHover) {
		shapeHover->mouseDown((float)pos.x, (float)pos.y);
		return;
	}
	shapeHover = history->createShape(toolMain->curId, pos.x, pos.y);
}

void WinPin::onMove(POINT pos)
{
	// 同 onDown：文本框里的移动归 TextBox（拖选、滚动条 hover），不参与 shape 的 hover 判定
	if (editingText && textBox && textBox->isPosIn(pos)) return;
	if (isMouseDown) {
		if (toolMain->curId == L"") {
			setPosition(x + pos.x - pressPos.x, y + pos.y - pressPos.y);
			return;
		}
		else if(shapeHover) {
			shapeHover->mouseDrag((float)pos.x, (float)pos.y);
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
			cur->mouseMove((float)pos.x, (float)pos.y);
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
	if (toolMain->curId == L"") { //state为空时，是在拖动窗口
		layoutTools();
		toolMain->show();
	}
	else if (shapeHover) {
		shapeHover->mouseUp((float)pos.x, (float)pos.y);
		refresh();
		setTimer(800, 100);
	}

}

void WinPin::onTimerCB(UINT id)
{
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
	if (!screenImg || w <= 0 || h <= 0) return false;
	// 编辑中的文字是 TextBox 自己那层画的，进不了下面这个离屏 target。
	// 先收尾，把文字交回 ShapeText 自己画，保存/复制出去的图才有它。
	if (editingText) editingText->finishEdit();
	auto size = D2D1::SizeU((UINT32)w, (UINT32)h);
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
	ctx->DrawBitmap(screenImg.Get(), D2D1::RectF(0, 0, w, h));
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
