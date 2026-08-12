#include "pch.h"
#include "../Win/WinCap.h"
#include "../Lang.h"
#include "../Tip.h"
#include "ToolLong.h"

ToolLong::ToolLong(WinCap* win) : Ling::WinBase(), win(win)
{
	auto btnSize{ 32.f * win->dpi };
	w = btnSize * btnIds.size();
	h = btnSize;
	// 位置由 CapLong::makeTool() 在 createNativeWindow 之前设好
	// 点按钮会把 ToolLong 激活，键盘消息进的是它，转发给 WinCap 让 ESC 一致生效
	onKeyDown.add([this](UINT key) { this->win->onKeyDown(key); });
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
		win->longSaveToFile();
	}
	win->close();
}

void ToolLong::onMinMaxInfo(MINMAXINFO* mmi)
{
	mmi->ptMinTrackSize.x = 1;
	mmi->ptMinTrackSize.y = 1;
}
