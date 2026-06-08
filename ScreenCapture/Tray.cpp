#include "pch.h"
#include "Tray.h"
#include "App.h"
#include "Win/WinCap.h"

std::unique_ptr<Tray> trayIns;

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
}

Tray* Tray::get()
{
	return trayIns.get();
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
	tray->uCallbackMessage = WM_APP + 100; //自定义窗口消息
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
	else if (msg == WM_APP + 666)
	{
		WinCap::init();
	}
    else if(msg == WM_APP + 100)
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
		if (menuId == 0)
		{
			WinCap::init();
		}
		else if (menuId == 1)
		{
			// 截长图
		}
		else if (menuId == 2)
		{
			// 录屏
		}
		else if (menuId == 3)
		{
			// 设置
		}
		else if (menuId == 4)
		{
			App::exit(0);
		}
	}
    return DefWindowProc(hWnd, msg, wParam, lParam);
}
void Tray::onTrayClick()
{

}
void Tray::onTrayRightClick()
{
	auto menu = CreatePopupMenu();
	AppendMenu(menu, MF_STRING, 0, L"截图");
	AppendMenu(menu, MF_STRING, 1, L"截长图");
	AppendMenu(menu, MF_STRING, 2, L"录屏");
	AppendMenu(menu, MF_SEPARATOR, 0, NULL);
	AppendMenu(menu, MF_STRING, 3, L"设置");
	AppendMenu(menu, MF_STRING, 4, L"退出");
	POINT pt;
	GetCursorPos(&pt); //获取鼠标位置
	TrackPopupMenuEx(menu, TPM_RIGHTBUTTON,  pt.x, pt.y, hwnd, nullptr);
	DestroyMenu(menu);
}