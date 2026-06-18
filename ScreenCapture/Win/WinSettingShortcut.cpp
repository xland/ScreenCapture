#include "pch.h"
#include "WinSettingShortcut.h"
#include "WinSetting.h"

WinSettingShortcut::WinSettingShortcut(WinSetting* win):win{win}
{
	iconSize *= win->dpi;
	lineH *= win->dpi;
	contentX = win->menuW + win->menuLeftSpan * 7;
	contentY = win->paddintTop + win->menuLeftSpan;
	contentR = win->w - win->menuLeftSpan * 7;
	win->render->CreateSolidColorBrush(D2D1::ColorF(0x1677FF), openBrush.GetAddressOf());
	auto w = contentR - contentX;
	startupLabel = win->makeTextLayout(L"开机自启动：", w, lineH, win->textSize, false);
	trayLabel = win->makeTextLayout(L"显示托盘图标：", w, lineH, win->textSize, false);
	langLabel = win->makeTextLayout(L"语言设置：（此功能尚未完成）", w, lineH, win->textSize, false);
	closeIcon = win->makeIconLayout(L"\ue687", lineH, lineH, iconSize);
	openIcon = win->makeIconLayout(L"\ue688", lineH, lineH, iconSize);
}

WinSettingShortcut::~WinSettingShortcut()
{

}

void WinSettingShortcut::paint()
{
	win->render->DrawTextLayout({ contentX,contentY }, startupLabel.Get(), win->textBrush2.Get());
	if (isAutoStart) {
		win->render->DrawTextLayout({ contentR - lineH,contentY }, openIcon.Get(), openBrush.Get());
	}
	else {
		win->render->DrawTextLayout({ contentR - lineH,contentY }, closeIcon.Get(), win->textBrush.Get());
	}
	auto y = contentY + lineH;
	win->render->DrawLine({ contentX,y }, { contentR,y }, win->border.Get(), win->dpi);
	win->render->DrawTextLayout({ contentX,y }, trayLabel.Get(), win->textBrush2.Get());
	if (isShowTray) {
		win->render->DrawTextLayout({ contentR - lineH,y }, openIcon.Get(), openBrush.Get());
	}
	else {
		win->render->DrawTextLayout({ contentR - lineH,y }, closeIcon.Get(), win->textBrush.Get());
	}

	y += lineH;
	win->render->DrawLine({ contentX,y }, { contentR,y }, win->border.Get(), win->dpi);
	win->render->DrawTextLayout({ contentX,y }, langLabel.Get(), win->textBrush.Get());
	win->render->DrawLine({ contentX,y + lineH }, { contentR,y + lineH }, win->border.Get(), win->dpi);
}
