#include "pch.h"
#include "../Win/WinCap.h"
#include "../Lang.h"
#include "../Tip.h"
#include "ToolCap.h"

ToolCap::ToolCap(WinCap* win) : Ling::WinBase(), win(win)
{
	// 跟着宿主窗口的缩放走：WinBase 构造里取的是系统 dpi，宿主可能在另一块缩放比例不同的屏上
	dpi = win->dpi;
	// 位置由 WinCap::layoutTool() 在 createNativeWindow 之前设好，这里只算尺寸
	refreshSize();
	// 点按钮会把 ToolCap 激活，键盘消息进的是它，转发给 WinCap 让 ESC 一致生效
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
		this->win->layoutTool(this);
	});
}

void ToolCap::refreshSize()
{
	float logicW{ 0.f };
	for (auto& id : btnIds) {
		logicW += (id == L"spliter") ? spliterW : btnSize;
	}
	setSize(logicW, btnSize);
}

ToolCap::~ToolCap()
{
}

void ToolCap::onCreated()
{
	tip = std::make_unique<Tip>(this);
	body->setBg(0xFFFFFFFF);
	body->setBorder(1.f, 0xA8A8A8ff);
	body->setAlignItems(Ling::Align::Center);
	body->setFlexDirection(Ling::FlexDirection::Row);
	for (size_t i = 0; i < btnIds.size(); i++)
	{
		if (btnIds[i] == L"spliter") {
			auto spliter = body->makeChild<Ling::Node>();
			spliter->setSize(spliterW, 18.f);
			spliter->setBg(0xDDDDDDff);
			continue;
		}
		auto btn = body->makeChild<Ling::Button>();
		btn->setId(btnIds[i]);
		btn->setText(btnCodes[i]);
		btn->setWidth(btnSize);
		btn->setHeightPercent(100.f);
		btn->setHoverBg(0xF2F2F2ff);
		btn->setFontFamily(L"icon");
		btn->setFontSize(13.f);
		btn->onClick.add([this](Ling::Button* btn) { onClick(btn); });
		if (!btnTips[i].empty()) {
			tip->bind(btn, Lang::get(btnTips[i]));
		}
	}
	show();
}

void ToolCap::onClick(Ling::Button* btn)
{
	// 提示框跟着按钮所在的窗口走，这里马上要换阶段或者关窗口，先把它收掉
	tip->hide();
	if (btn->id == L"mark") {
		win->startPin();
	}
	else if (btn->id == L"long") {
		win->startLong();
	}
	else if (btn->id == L"video") {
		win->startVideo();
	}
	else if (btn->id == L"ocr") {
		win->startOcr();
	}
	else if (btn->id == L"qrcode") {
		win->startQrcode();
	}
	else if (btn->id == L"save") {
		win->saveToFile();
	}
	else if (btn->id == L"clipboard") {
		win->copyToClipboard();
	}
	else if (btn->id == L"close") {
		win->close();
	}
}

void ToolCap::onMinMaxInfo(MINMAXINFO* mmi)
{
	mmi->ptMinTrackSize.x = 1;
	mmi->ptMinTrackSize.y = 1;
}
