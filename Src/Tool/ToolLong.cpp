#include "pch.h"
#include "../Win/WinCap.h"
#include "../Lang.h"
#include "../Tip.h"
#include "ToolLong.h"

ToolLong::ToolLong(WinCap* win) : Ling::WinBase(), win(win)
{
	// 跟着宿主窗口的缩放走：WinBase 构造里取的是系统 dpi，宿主可能在另一块缩放比例不同的屏上
	dpi = win->dpi;
	// 位置由 CapLong::makeTool() 在 createNativeWindow 之前设好，这里只算尺寸
	refreshSize();
	// 点按钮会把 ToolLong 激活，键盘消息进的是它，转发给 WinCap 让 ESC 一致生效
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
		this->win->layoutLongTool();
	});
}

void ToolLong::refreshSize()
{
	setSize(btnSize * static_cast<float>(btnIds.size()), btnSize);
}

ToolLong::~ToolLong()
{
}

void ToolLong::onCreated()
{
	tip = std::make_unique<Tip>(this);
	body->setBg(0xFFFFFFFF);
	body->setBorder(1.f, 0xA8A8A8ff);
	body->setAlignItems(Ling::Align::Center);
	body->setFlexDirection(Ling::FlexDirection::Row);
	for (size_t i = 0; i < btnIds.size(); i++)
	{
		auto btn = body->makeChild<Ling::Button>();
		btn->setId(btnIds[i]);
		btn->setText(btnCodes[i]);
		btn->setHeightPercent(100.f);
		btn->setFlexGrow(1.f);
		btn->setHoverBg(0xF2F2F2ff);
		btn->setFontFamily(L"icon");
		btn->setFontSize(13.f);
		btn->onClick.add([this](Ling::Button* btn) {onClick(btn);});
		tip->bind(btn, Lang::get(std::format(L"tool.{}", btnIds[i])));
	}
	show();
}

void ToolLong::onClick(Ling::Button* btn)
{
	// 四个按钮都是"做完就收工"，做完动作后统一关掉整个滚动截图流程
	if (btn->id == L"pin") {
		win->longPin();
	}
	else if (btn->id == L"clipboard") {
		win->longCopyToClipboard();
	}
	else if (btn->id == L"save") {
		// 唯一的例外：另存为对话框被取消了，滚了半天的图还在，别就这么关掉
		if (!win->longSaveToFile()) return;
	}
	win->close();
}

void ToolLong::onMinMaxInfo(MINMAXINFO* mmi)
{
	mmi->ptMinTrackSize.x = 1;
	mmi->ptMinTrackSize.y = 1;
}
