#include "pch.h"
#include "ToolMain.h"
#include "../Win/WinPin.h"

ToolMain::ToolMain(WinPin* win) : Ling::WinBase(), win(win)
{
	auto btnSize{ 32.f * win->dpi };
	std::vector<std::wstring> btnIds = { L"rect",L"ellipse",L"arrow",L"number",L"line",L"text",L"mosaic", L"eraser",L"undo",L"redo",L"close",L"save",L"clipboard" };
	x = win->x;
	y = win->y + win->h + 5.f * win->dpi;
	w = btnSize * btnIds.size()+ 2.f * win->dpi;
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

void ToolMain::onCreated()
{
	enableShadow();
	body->setBg(0xFFFFFFFF);
	body->setBorder(1.f, 0x1677ffff);
	body->setAlignItems(Ling::Align::Center);

	std::vector<std::wstring> btnCodes = { L"\ue8e8",L"\ue6bc",L"\ue603",L"\ue776",L"\ue601",
L"\ue6ec",L"\ue82e",L"\ue6be",L"|",L"\ued85",L"\ued8a",L"|",L"\ue62d",L"\ue608",L"\ue6ad"};
	body->setFlexDirection(Ling::FlexDirection::Row);
	int index{ 0 };
	for (auto& code : btnCodes)
	{
		if (code == L"|") {
			auto spliter = body->makeChild<Ling::Node>();
			spliter->setSize(dpi, 18.f);
			spliter->setBg(0xDDDDDDff);
		}
		else {
			auto btn = body->makeChild<Ling::Button>();
			btn->setText(code);
			btn->setHeightPercent(100.f);
			btn->setFlexGrow(1.f);
			btn->setBg(0xFFFFFFFF);
			btn->setHoverBg(0xe6f4ffff);
			btn->setFontFamily(L"icon");
			btn->setFontSize(13.f);
		}
		index += 1;
	}
	show();
}
