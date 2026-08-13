#include "pch.h"
#include "../Win/WinPin.h"
#include "../History.h"
#include "../Lang.h"
#include "../Tip.h"
#include "ToolMain.h"
#include "ToolSub.h"

ToolMain::ToolMain(WinPin* win) : Ling::WinBase(), win(win)
{
	// 跟着宿主窗口的缩放走：WinBase 构造里取的是系统 dpi，WinPin 可能在另一块缩放比例不同的屏上
	dpi = win->dpi;
	// 初始位置由 WinPin::layoutTools() 统一决定，这里只算尺寸
	x = win->x;
	y = win->y + win->h + 5.f * win->dpi;
	refreshSize();
	// 点按钮会把 ToolMain 激活，此后键盘消息进的是它而不是 WinPin。
	// 直接把按键转触给 WinPin 的同名事件，快捷键在两个窗口上表现一致。
	onKeyDown.add([this](UINT key) { this->win->onKeyDown(key); });
	// DPI 变了（工具条被挪到缩放比例不同的显示器上，或者用户改了系统缩放）：
	// Ling 只会把窗口按系统给的建议矩形整体缩放一遍，我们自己定的那套摆放规则不会重跑，
	// 工具条就歪在别处了。位置也不能在 onDpiChanged 里直接改 —— 那个事件在 Ling 应用建议矩形
	// 之前触发，改了马上被覆盖，所以这里只记个标记，等建议矩形应用后紧随而来的 WM_SIZE 再动手
	onDpiChanged.add([this]() { dpiChanged = true; });
	onSizeChanged.add([this]() {
		if (!dpiChanged) return;
		dpiChanged = false;
		refreshSize();                    //宿主的摆放规则要用宽高，先按新 dpi 把尺寸定下来
		this->win->layoutTools();
	});
	createNativeWindow(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, WS_POPUP);
}

// 宽度 = 各按钮宽度之和（btnIds 里的 "|" 是分隔符，只占 spliterW），高度 = 按钮高
void ToolMain::refreshSize()
{
	float logicW{ 0.f };
	for (auto& id : btnIds) {
		logicW += (id == L"|") ? spliterW : btnSize;
	}
	setSize(logicW, btnSize);
}

ToolMain::~ToolMain()
{
}

void ToolMain::init()
{
}

// 返回 curId 对应按钮的中心相对 ToolMain 左边的偏移（物理像素）。
// btnIds 含分隔符而 btns 不含，所以要单独维护 btns 的下标，不能拿 i 去索引 btns。
float ToolMain::getBtnCenterX()
{
	float result{ 0.f };
	size_t btnIndex{ 0 };
	for (size_t i = 0; i < btnIds.size(); i++)
	{
		if (btnIds[i] == L"|") {
			// 分隔符不在 btns 里，宽度与 onCreated 里 spliter 的 setSize(dpi, ...) 一致
			result += dpi;
			continue;
		}
		if (curId == btnIds[i]) {
			result += btns[btnIndex]->w / 2.f;
			return result;
		}
		result += btns[btnIndex]->w;
		btnIndex++;
	}
	return result;
}

void ToolMain::onCreated()
{
	tip = std::make_unique<Tip>(this);
	body->setBg(0xFFFFFFFF);
	body->setBorder(1.f, 0xA8A8A8ff);
	body->setAlignItems(Ling::Align::Center);
	body->setFlexDirection(Ling::FlexDirection::Row);
	for (size_t i = 0; i < btnIds.size(); i++)
	{
		auto& id = btnIds[i];
		if (id == L"|") {
			auto spliter = body->makeChild<Ling::Node>();
			spliter->setSize(dpi, 18.f);
			spliter->setBg(0xDDDDDDff);
		}
		else {
			auto btn = body->makeChild<Ling::Button>();
			btn->setId(id);
			btn->setText(btnCodes[i]);
			btn->setHeightPercent(100.f);
			btn->setFlexGrow(1.f);
			btn->setHoverBg(0xF2F2F2ff);
			btn->setFontFamily(L"icon");
			btn->setFontSize(13.f);
			btn->onClick.add([this](Ling::Button* btn) {onClick(btn);});
			tip->bind(btn, Lang::get(std::format(L"tool.{}", id)));
			btns.push_back(btn);
		}
	}
	show();
}

void ToolMain::applyNormalStyle(Ling::Button* btn)
{
	btn->setBg(0);
	btn->setHoverBg(0xF2F2F2ff);
}

// 取消选中：与 onClick 选中某个按钮是对称操作，只是没有新的选中项。
// ToolSub 由 curId 是否为空驱动，所以清空 curId 后 layoutTools() 会自动把它收起来。
void ToolMain::cancelSelect()
{
	if (curId.empty()) return;
	for (auto b : btns)
	{
		if (b->id == curId) {
			applyNormalStyle(b);
		}
	}
	curId.clear();
	win->toolSub->hideTools();
	// curId 空了 ToolMain 要下移收回 ToolSub 让出的空间，交给 WinPin 重排整组
	win->layoutTools();
}

void ToolMain::onClick(Ling::Button* btn)
{
	// 关闭整个贴图窗口。WinPin 的 onDestroy 里会连带关掉 ToolMain / ToolSub，
	// 但 C++ 对象的释放被推迟到下一轮消息循环，所以这里 return 之后栈上访问 this 仍是安全的。
	if (btn->id == L"close") {
		win->close();
		return;
	}
	// 下面这几个都是"执行一次动作"而不是"切换绘图工具"，做完就返回，不动 curId 和选中态。
	// undo/redo 由 History 内部负责 refresh；save/clipboard 成功后会关窗，同样不能往下走。
	else if (btn->id == L"undo") {
		win->history->undo();
		return;
	}
	else if (btn->id == L"redo") {
		win->history->redo();
		return;
	}
	else if (btn->id == L"save") {
		win->saveToFile();
		return;
	}
	else if (btn->id == L"clipboard") {
		win->copyToClipboard();
		return;
	}
	// 再次点击已选中的按钮 = 取消选中（开关式）。cancelSelect 里已经做了配色复位、
	// 隐藏 ToolSub 和重排，这里直接返回，不要再往下走选中流程。
	if (btn->id == curId) {
		cancelSelect();
		return;
	}
	for (auto b:btns)
	{
		if (b->id == curId)
		{
			applyNormalStyle(b);
		}
		if (b->id == btn->id)
		{
			b->setBg(0xe6f4ffff);
			b->setHoverBg(0xe6f4ffff);
		}
	}
	curId = btn->id;
	if (curId == L"rect") {
		win->toolSub->showRectTools();
	}
	else if (curId == L"ellipse") {
		win->toolSub->showEllipseTools();
	}
	else if (curId == L"arrow") {
		win->toolSub->showArrowTools();
	}
	else if (curId == L"number") {
		win->toolSub->showNumberTools();
	}
	else if (curId == L"line") {
		win->toolSub->showLineTools();
	}
	else if (curId == L"text") {
		win->toolSub->showTextTools();
	}
	else if (curId == L"mosaic") {
		win->toolSub->showMosaicTools();
	}
	else if (curId == L"eraser") {
		win->toolSub->showEraserTools();
	}
	else {
		win->toolSub->hideTools();
	}
	// curId 变化后 ToolMain 可能要上移给 ToolSub 腾位置，交给 WinPin 重新排布整组
	win->layoutTools();
}

void ToolMain::onMinMaxInfo(MINMAXINFO* mmi)
{
	mmi->ptMinTrackSize.x = 1;
	mmi->ptMinTrackSize.y = 1;
}