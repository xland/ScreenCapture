#include "pch.h"
#include "Tray.h"
#include "App.h"
#include "Win/WinCap.h"

namespace {
	std::unique_ptr<Tray> trayIns;
	static constexpr UINT secondInsMsg = WM_APP + 560;
	static constexpr UINT trayClickMsg = WM_APP + 561;
	static constexpr UINT capScreenMsg = WM_APP + 562;
	static constexpr UINT capLongMsg = WM_APP + 563;
	static constexpr UINT capVideoMsg = WM_APP + 564;
	static constexpr UINT settingMsg = WM_APP + 565;
	static constexpr UINT exitMsg = WM_APP + 566;
	static constexpr UINT capScreenKeyMsg = WM_APP + 567;
	static constexpr UINT capLongKeyMsg = WM_APP + 568;
	static constexpr UINT capVideoKeyMsg = WM_APP + 569;
}


Tray::Tray()
{

}

Tray::~Tray()
{

}

void Tray::init()
{
	trayIns = std::make_unique<Tray>();
	trayIns->createWin();
    trayIns->createTray();
	trayIns->regHotKey();
}

Tray* Tray::get()
{
	return trayIns.get();
}

void Tray::regHotKey()
{
	BOOL result = RegisterHotKey(hwnd, capScreenKeyMsg, MOD_CONTROL | MOD_ALT, 'A');
	if (!result) {
		DWORD error = GetLastError();
		if (error == ERROR_HOTKEY_ALREADY_REGISTERED) {

		}
	}
}

void Tray::createWin()
{
	hwnd = CreateWindow(L"STATIC", L"ScreenCaptureMsgWnd", 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, App::get()->hInstance, NULL);
	SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)Tray::wndProc);
	SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
}

void Tray::createTray()
{
	tray = std::make_unique<NOTIFYICONDATA>();
	ZeroMemory(tray.get(), sizeof(NOTIFYICONDATA));
	tray->cbSize = sizeof(NOTIFYICONDATA);
	tray->hWnd = hwnd; //窗口句柄
	tray->uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	tray->uCallbackMessage = trayClickMsg; //自定义窗口消息
	tray->hIcon = LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(100)); //使用当前exe文件的图标
	wcscpy_s(tray->szTip, L"Screen Capture");
	Shell_NotifyIcon(NIM_ADD, tray.get()); //添加托盘图标
}


LRESULT CALLBACK Tray::wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	auto self = reinterpret_cast<Tray*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	if (!self) {
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	else if (msg == secondInsMsg)
	{
		WinCap::init();
	}
	else if (msg == WM_HOTKEY) {
		if (wParam == capScreenKeyMsg) {
			WinCap::init();
		}
		return 0;
	}
    else if(msg == trayClickMsg)
    {
		if (lParam == WM_LBUTTONDOWN) {
			self->onTrayClick();
		}
		else if (lParam == WM_RBUTTONDOWN)
		{
			self->onTrayRightClick();
		}
    }
	else if (msg == WM_COMMAND) {
		int menuId = LOWORD(wParam);
		if (menuId == capScreenMsg)
		{
			WinCap::init();
		}
		else if (menuId == capLongMsg)
		{
			// 截长图
		}
		else if (menuId == capVideoMsg)
		{
			// 录屏
		}
		else if (menuId == settingMsg)
		{
			// 设置
		}
		else if (menuId == exitMsg)
		{
			App::exit(0);
		}
	}
    return DefWindowProc(hWnd, msg, wParam, lParam);
}
void Tray::onTrayClick()
{
	WinCap::init();
}
void Tray::onTrayRightClick()
{
	auto menu = CreatePopupMenu();
	AppendMenu(menu, MF_STRING, capScreenMsg, L"截图");
	AppendMenu(menu, MF_STRING, capLongMsg, L"截长图");
	AppendMenu(menu, MF_STRING, capVideoMsg, L"录屏");
	AppendMenu(menu, MF_SEPARATOR, 0, NULL);
	AppendMenu(menu, MF_STRING, settingMsg, L"设置");
	AppendMenu(menu, MF_STRING, exitMsg, L"退出");
	POINT pt;
	GetCursorPos(&pt); //获取鼠标位置
	TrackPopupMenuEx(menu, TPM_RIGHTBUTTON,  pt.x, pt.y, hwnd, nullptr);
	DestroyMenu(menu);
}

bool Tray::secondIns()
{
	auto hwnd = FindWindow(L"STATIC", L"ScreenCaptureMsgWnd");
	if (hwnd) {
		PostMessage(hwnd, secondInsMsg, 0, 0);
		App::exit(0);
		return true;
	}
	return false;
}