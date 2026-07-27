#include "pch.h"
#include "Tray.h"
#include "App.h"
#include "Lang.h"
//#include "Win/WinCap.h"
//#include "Win/WinVideo.h"
//#include "Win/WinLong.h"
//#include "Win/WinSetting.h"
#include "Setting.h"

namespace {
	std::unique_ptr<Tray> trayIns;
	static constexpr UINT secondInsMsg = 560;
	static constexpr UINT trayClickMsg = 561;
	static constexpr UINT capScreenMsg = 562;
	static constexpr UINT capLongMsg = 563;
	static constexpr UINT capVideoMsg = 564;
	static constexpr UINT settingMsg = 565;
	static constexpr UINT exitMsg = 566;
	static constexpr UINT capScreenKeyMsg = 567;
	static constexpr UINT capLongKeyMsg = 568;
	static constexpr UINT capVideoKeyMsg = 569;
}


Tray::Tray()
{
	auto lingApp = Ling::App::get();
	lingApp->initTray(100, L"Screen Capture");
	regHotKeys();
	lingApp->onHotKey.add([this](UINT msgId) {
		if (msgId == capScreenKeyMsg) {
			//WinCap::init();
		}
		else if (msgId == capLongKeyMsg) {
			//WinLong::init();
		}
		else if (msgId == capVideoKeyMsg) {
			//WinVideo::init();
		}
	});
	lingApp->onSecondInstance.add([this]() {
		//WinCap::init();
	});
	lingApp->onTrayMouseEvent.add([this](bool isDown, bool isRight) {
		if (isDown && !isRight) {
			//WinCap::init();
		}
		else if (isDown && isRight) {
			this->onTrayRightClick();
		}
	});

}

Tray::~Tray()
{
	unregHotKeys();
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


void Tray::unregHotKeys()
{
	auto lingApp = Ling::App::get();
	lingApp->unRegHotKey(capScreenMsg);
	lingApp->unRegHotKey(capLongKeyMsg);
	lingApp->unRegHotKey(capVideoKeyMsg);
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
void Tray::regHotKeys()
{
	auto lingApp = Ling::App::get();
	auto configShortcut = Setting::get()->getConfigObj().GetNamedObject(L"shortcutKey");
	std::wstring capStr{ configShortcut.GetNamedString(L"cap") };
	lingApp->regHotKey(capStr, capScreenMsg);
	std::wstring longStr{ configShortcut.GetNamedString(L"long") };
	lingApp->regHotKey(longStr, capLongKeyMsg);
	std::wstring videoStr{ configShortcut.GetNamedString(L"video") };
	lingApp->regHotKey(videoStr, capVideoKeyMsg);
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
		//WinCap::init();
	}
	else if (menuId == capLongMsg)
	{
		//WinLong::init();
	}
	else if (menuId == capVideoMsg)
	{
		//WinVideo::init();
	}
	else if (menuId == settingMsg)
	{
		//WinSetting::init();
	}
	else if (menuId == exitMsg)
	{
		Ling::App::get()->quit(0);
	}
}
