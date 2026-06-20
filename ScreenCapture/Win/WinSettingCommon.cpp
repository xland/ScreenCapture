#include "pch.h"
#include "WinSettingCommon.h"
#include "WinSetting.h"
#include "Setting.h"
#include "Util.h"
#include "Tray.h"

WinSettingCommon::WinSettingCommon(WinSetting* win):win{win}
{
	iconSize *= win->dpi;
	lineH *= win->dpi;
	contentX = win->menuW + win->menuLeftSpan * 7;
	contentY = win->paddintTop + win->menuLeftSpan;
	contentR = win->w - win->menuLeftSpan * 7;

	auto configObj = Setting::getConfigObj();
	auto common = configObj.GetNamedObject(L"common");
	isShowTray = common.GetNamedBoolean(L"showTray");
	isAutoStart = common.GetNamedBoolean(L"autoStart");

	win->render->CreateSolidColorBrush(D2D1::ColorF(0x1677FF), openBrush.GetAddressOf());
	auto w = contentR - contentX;
	startupLabel = win->makeTextLayout(L"开机自启动：", w, lineH, win->textSize, false);
	trayLabel = win->makeTextLayout(L"显示托盘图标：", w, lineH, win->textSize, false);
	langLabel = win->makeTextLayout(L"语言设置：（此功能尚未完成）", w, lineH, win->textSize, false);
	closeIcon = win->makeIconLayout(L"\ue687", lineH, lineH, iconSize);
	openIcon = win->makeIconLayout(L"\ue688", lineH, lineH, iconSize);
}

WinSettingCommon::~WinSettingCommon()
{

}

void WinSettingCommon::paint()
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

void WinSettingCommon::mouseMove(const int& x, const int& y)
{
	hoverIndex = -1;
	if (x > contentR - lineH && x<contentR && y>contentY) {
		if (y < contentY + lineH) hoverIndex = 0;
		else if (y < contentY + 2 * lineH) hoverIndex = 1;
	}
}

void WinSettingCommon::mouseDown()
{
	if (hoverIndex == 0) {
		isAutoStart = !isAutoStart;
		Util::setAutoStart(isAutoStart);
		Setting::setSwitch(isAutoStart, isShowTray);
		win->refresh();
	}
	else if (hoverIndex == 1) {
		isShowTray = !isShowTray;
		Tray::get()->setTray(isShowTray);
		Setting::setSwitch(isAutoStart, isShowTray);
		win->refresh();
	}
}
