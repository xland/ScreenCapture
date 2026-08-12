#include "pch.h"
#include "../Win/WinCap.h"
#include "../Lang.h"
#include "../Tip.h"
#include "ToolCap.h"

ToolCap::ToolCap(WinCap* win) : Ling::WinBase(), win(win)
{
	// 位置由 WinCap::layoutTool() 在 createNativeWindow 之前设好，这里只算尺寸。
	// 不走 setSize：它会把参数当逻辑像素再乘一遍 dpi。
	float logicW{ 0.f };
	for (auto& id : btnIds) {
		logicW += (id == L"spliter") ? spliterW : btnSize;
	}
	w = logicW * win->dpi;
	h = btnSize * win->dpi;
	// 点按钮会把 ToolCap 激活，键盘消息进的是它，转发给 WinCap 让 ESC 一致生效
	onKeyDown.add([this](UINT key) { this->win->onKeyDown(key); });
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
