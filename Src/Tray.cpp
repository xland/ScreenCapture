#include "pch.h"
#include "Tray.h"
#include "App.h"
#include "Lang.h"
#include "Win/WinCap.h"
#include "Win/WinVideo.h"
#include "Win/WinLong.h"
#include "Win/WinSetting.h"
#include "Setting.h"

namespace {
	std::unique_ptr<Tray> trayIns;
	static constexpr UINT capScreenMsg = 160;
	static constexpr UINT capLongMsg = 161;
	static constexpr UINT capVideoMsg = 162;
	static constexpr UINT settingMsg = 163;
	static constexpr UINT exitMsg = 164;
}


Tray::Tray()
{
	auto lingApp = Ling::App::get();
	lingApp->initTray(100, L"Screen Capture");
	Setting::get()->initShortcutKeys();

	lingApp->onTrayMouseEvent.add([this](bool isDown, bool isRight) {
		if (isDown && !isRight) {
			WinCap::init();
		}
		else if (isDown && isRight) {
			this->onTrayRightClick();
		}
	});

}

Tray::~Tray()
{
}
void Tray::init()
{
	auto ptr = new Tray();
	trayIns.reset(ptr);
}

Tray* Tray::get()
{
	return trayIns.get();
}

LRESULT CALLBACK Tray::wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	auto self = reinterpret_cast<Tray*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	if (!self) {
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	else if (msg == WM_COMMAND) {
		int menuId = LOWORD(wParam);
		
	}
    return DefWindowProc(hWnd, msg, wParam, lParam);
}
void Tray::onTrayRightClick()
{
	auto menu = CreatePopupMenu();
	AppendMenu(menu, MF_STRING, capScreenMsg, Lang::get(L"tray.capScreen").data());
	AppendMenu(menu, MF_STRING, capLongMsg, Lang::get(L"tray.capLong").data());
	AppendMenu(menu, MF_STRING, capVideoMsg, Lang::get(L"tray.capVideo").data());
	AppendMenu(menu, MF_SEPARATOR, 0, NULL);
	AppendMenu(menu, MF_STRING, settingMsg, Lang::get(L"tray.setting").data());
	AppendMenu(menu, MF_STRING, exitMsg, Lang::get(L"tray.exit").data());
	auto menuId = Ling::App::get()->popupMenu(menu);
	if (menuId == capScreenMsg)
	{
		WinCap::init();
	}
	else if (menuId == capLongMsg)
	{
		WinLong::init();
	}
	else if (menuId == capVideoMsg)
	{
		WinVideo::init();
	}
	else if (menuId == settingMsg)
	{
		WinSetting::init();
	}
	else if (menuId == exitMsg)
	{
		Ling::App::get()->quit(0);
	}
}
