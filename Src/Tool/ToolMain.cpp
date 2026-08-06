#include "pch.h"
#include "../Win/WinPin.h"
#include "ToolMain.h"
#include "ToolSub.h"

ToolMain::ToolMain(WinPin* win) : Ling::WinBase(), win(win)
{
	auto btnSize{ 32.f * win->dpi };
	x = win->x;
	y = win->y + win->h + 5.f * win->dpi;
	w = btnSize * btnIds.size()+ 2.f * win->dpi; //有两个分隔符
	h = btnSize;
	createNativeWindow(WS_EX_TOPMOST | WS_EX_NOACTIVATE, WS_POPUP);
	win->onMoved.add([this,win]() {
		this->setPosition(win->x, win->y + win->h + 5.f * win->dpi);
	});
}

ToolMain::~ToolMain()
{
}

void ToolMain::init()
{
}

float ToolMain::getBtnCenterX()
{
	float result{ 0.f };
	for (size_t i = 0; i < btnIds.size(); i++)
	{
		if (curId == L"|") {
			result += dpi;
		}
		if (curId == btnIds[i]) {
			result += btns[i]->w / 2.f;
			break;
		}
		else {
			result += btns[i]->w;
		}
	}
	return result;
}

void ToolMain::onCreated()
{
	//enableShadow();
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
			btns.push_back(btn);
		}
	}
	show();
}

void ToolMain::onClick(Ling::Button* btn)
{
	for (auto b:btns)
	{
		if (b->id == curId)
		{
			b->setBg(0);
			b->setHoverBg(0xF2F2F2ff);
		}
		if (b->id == btn->id)
		{
			b->setBg(0xe6f4ffff);
			b->setHoverBg(0xe6f4ffff);
		}
	}
	curId = btn->id;
	win->toolSub->setPosAndShow();
}

void ToolMain::onMinMaxInfo(MINMAXINFO* mmi)
{
	mmi->ptMinTrackSize.x = 1;
	mmi->ptMinTrackSize.y = 1;
}