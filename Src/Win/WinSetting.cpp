#include "pch.h"
#include <filesystem>
#include "../App.h"
#include "../Lang.h"
#include "WinSetting.h"
#include "WinSettingCommon.h"
#include "WinSettingShortcut.h"
#include "WinSettingAbout.h"

std::unique_ptr<WinSetting> winSetting;

WinSetting::WinSetting() :Ling::WinBase()
{
	setTitle(Lang::get(L"setting.title"));
	setSize(680, 560);
	setCenter();
	createNativeWindow();
}

WinSetting::~WinSetting()
{

}

void WinSetting::init()
{
	if (winSetting) winSetting->close();
	auto ptr = new WinSetting();
	winSetting.reset(ptr);
}

void WinSetting::onCreated()
{
	enableShadow();
	body->setBg(0xFAFAFAFF);
	body->setFlexDirection(Ling::FlexDirection::Row);
	auto menuBox = body->makeChild<Ling::Node>();
	menuBox->setBg(0xEEEEF0FF);
	menuBox->setWidth(160.f);
	menuBox->setHeightPercent(100.f);
	menuBox->setPaddingTop(40.f);
	initMenuItems(menuBox);

	content = body->makeChild<WinSettingCommon>();
	content->setFlexGrow(1.0);
	content->setHeightPercent(100.f);
	content->setPaddingTop(40.f);
	content->setPadding(20.f, 40.f, 20.f, 40.f);
	content->setFlexDirection(Ling::FlexDirection::Column);

	auto closeBtn = body->makeChild<Ling::Button>();
	closeBtn->setSize(42.f, 32.f);
	closeBtn->setPositionType(Ling::Position::Absolute);
	closeBtn->setPosition(Ling::Edge::Right, 0);
	closeBtn->setPosition(Ling::Edge::Top, 0);
	closeBtn->setHoverColor(0xFFFFFFFF);
	closeBtn->setHoverBg(0xE81123FF);
	closeBtn->setText(L"\ue62d");
	closeBtn->setFontFamily(L"icon");
	closeBtn->onClick.add([](Ling::Button* btn) {
		btn->win->close();
		});
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
			menuItem->setBg(0x597ef7ff);
			menuItem->setHoverColor(0xFFFFFFFF);
			menuItem->setHoverBg(0x597ef7ff);
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
	// 通用设置里的语言下拉框是挂在 body 上的（要能盖住下面的控件），content 被换掉
	// 它不会跟着消失，所以切菜单之前先收掉
	if (menuIndex == 0) {
		static_cast<WinSettingCommon*>(content)->hideSelectBox();
	}
	auto oldItem = menus[menuIndex];
	oldItem->setColor(0x333333FF);
	oldItem->setBg(0x00000000);
	oldItem->setHoverColor(0x000000ff);
	oldItem->setHoverBg(0xE1E1E3ff);
	menuIndex = index;
	menuItem->setColor(0xFFFFFFFF);
	menuItem->setBg(0x597ef7ff);
	menuItem->setHoverColor(0xFFFFFFFF);
	menuItem->setHoverBg(0x597ef7ff);

	body->removeChild(content);
	if (menuIndex == 0) {
		content = body->makeChild<WinSettingCommon>();
	}
	else if (menuIndex == 1) {
		content = body->makeChild<WinSettingShortcut>();
	}
	else if (menuIndex == 2) {
		content = body->makeChild<WinSettingAbout>();
	}
	content->setFlexGrow(1.0);
	content->setHeightPercent(100.f);
	content->setPadding(20.f,40.f,20.f,40.f);
	content->setFlexDirection(Ling::FlexDirection::Column);
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


