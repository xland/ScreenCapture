#include "pch.h"
#include "../App.h"
#include "WinSetting.h"
#include "../Lang.h"

std::unique_ptr<WinSetting> winSetting;

WinSetting::WinSetting() :Ling::WinBase()
{
	setTitle(Lang::get(L"setting.title"));
	setSize(800, 600);
	setCenter();
	createNativeWindow();
}



WinSetting::~WinSetting()
{

}

void WinSetting::init()
{
	auto ptr = new WinSetting();
	winSetting.reset(ptr);
}

void WinSetting::onCreated()
{
	enableShadow();
	body->setBg(0xFAFAFAFF);
	auto menuBox = body->makeChild<Ling::Node>();
	menuBox->setBg(0xEEEEF0FF);
	menuBox->setWidth(120.f);
	menuBox->setHeightPercent(100.f);
	menuBox->setPaddingTop(40.f);
	initMenuItems(menuBox);

	auto closeBtn = body->makeChild<Ling::Button>();
	closeBtn->setSize(42.f, 40.f);
	closeBtn->setPositionType(Ling::Position::Absolute);
	closeBtn->setPosition(Ling::Edge::Right, 0);
	closeBtn->setPosition(Ling::Edge::Top, 0);
	closeBtn->setText(L"\ue62d");
	closeBtn->setFontFamily(L"icon");
	show();
}
void WinSetting::initMenuItems(Ling::Node* menuBox)
{
	for (size_t i = 0; i < 3; i++)
	{
		auto menuItem = menuBox->makeChild<Ling::Button>();
		menuItem->setFontSize(14.f);
		menuItem->setHeight(40.f);
		if (i == 0) {
			menuItem->setColor(0xFFFFFFFF);
			menuItem->setBg(0x2f54ebff);
			menuItem->setHoverColor(0xFFFFFFFF);
			menuItem->setHoverBg(0x2f54ebff);
			menuItem->setText(Lang::get(L"setting.common"));
		}
		else {
			menuItem->setHoverColor(0x000000ff);
			menuItem->setHoverBg(0xE1E1E3ff);
			if (i == 1) {
				menuItem->setText(Lang::get(L"setting.shortcut"));
			}
			else if (i == 2) {
				menuItem->setText(Lang::get(L"setting.about"));
			}
		}
		menuItem->onClick.add([this](auto menuItem) {this->onMenuItemClick(menuItem);});
		menus.push_back(menuItem);
	}
}
void WinSetting::onMenuItemClick(Ling::Button* menuItem)
{
	auto index = Ling::Util::getIndex(menus, menuItem);
	if (index < 0 || index == menuIndex) return;
	auto oldItem = menus[menuIndex];
	oldItem->setColor(0x333333FF);
	oldItem->setBg(0x00000000);
	oldItem->setHoverColor(0x000000ff);
	oldItem->setHoverBg(0xE1E1E3ff);
	menuIndex = index;
	menuItem->setColor(0xFFFFFFFF);
	menuItem->setBg(0x2f54ebff);
	menuItem->setHoverColor(0xFFFFFFFF);
	menuItem->setHoverBg(0x2f54ebff);
}

LRESULT WinSetting::onHitTest(const POINT pos)
{
	POINT pt = pos;
	ScreenToClient(hwnd, &pt);
	if (!isMaximized) {
		auto result = borderHitTest(pt);
		if (result != HTCLIENT) return result;
	}
	if (pt.x > 0 && pt.y > 0 && pt.x < w - 32 * dpi && pt.y < 40 * dpi) {
		return HTCAPTION;
	}
	if (pt.x > 0 && pt.y > 40*4*dpi && pt.x < 120 * dpi && pt.y < h) {
		return HTCAPTION;
	}
	return HTCLIENT;
}


