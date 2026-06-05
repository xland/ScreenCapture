#include "pch.h"
#include "Tray.h"
#include "App.h"

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
	hwnd = CreateWindow(L"STATIC", L"MsgWnd", 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, App::get()->hInstance, NULL);
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

void Tray::onTrayClick()
{

}

LRESULT CALLBACK Tray::wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	auto self = reinterpret_cast<Tray*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	if (!self) {
		return DefWindowProc(hWnd, msg, wParam, lParam);
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
    return DefWindowProc(hWnd, msg, wParam, lParam);
}
void Tray::onTrayRightClick()
{
	auto menu = CreatePopupMenu(); //创建弹出菜单
	AppendMenu(menu, MF_STRING, 0, L"显示窗口"); //设置菜单项
	AppendMenu(menu, MF_STRING, 1, L"测试菜单");
	AppendMenu(menu, MF_STRING, 2, L"退出");
	POINT pt;
	GetCursorPos(&pt); //获取鼠标位置
	//显示菜单（阻塞方法）
	int cmd = TrackPopupMenuEx(menu, TPM_RETURNCMD | TPM_RIGHTBUTTON, pt.x, pt.y, hwnd, nullptr);
	if (cmd == 0) {
		ShowWindow(hwnd, SW_SHOW);
		SetForegroundWindow(hwnd);
	}
	else if (cmd == 2) {
		App::exit(0);
	}
	DestroyMenu(menu);
}