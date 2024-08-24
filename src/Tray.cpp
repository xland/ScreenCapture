#include "Tray.h"
#include "Cmd.h"
#include <Windows.h>
#include "../res/res.h"

#define TRAY_ICON_ID 1
#define WM_TRAYICON (WM_USER + 1)

Tray::Tray()
{
}

// Ìí¼ÓÍÐÅÌÍ¼±ê
void addTrayIcon() {
    NOTIFYICONDATA nid = {};
    nid.hWnd = nullptr;
    nid.uID = TRAY_ICON_ID;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;
    nid.hIcon = (HICON)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_LOGO));
    lstrcpy(nid.szTip, L"Tray Icon Example");
    Shell_NotifyIcon(NIM_ADD, &nid);
}

// É¾³ýÍÐÅÌÍ¼±ê
void RemoveTrayIcon() {
    NOTIFYICONDATA nid = {};
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = nullptr;
    nid.uID = TRAY_ICON_ID;
    Shell_NotifyIcon(NIM_DELETE, &nid);
}

void Tray::Init()
{
	auto cmd = Cmd::Get();
	auto val = cmd->GetVal(L"shortcut");
    if (val.empty()) {
        return;
    }
    addTrayIcon();
}

Tray::~Tray()
{
}