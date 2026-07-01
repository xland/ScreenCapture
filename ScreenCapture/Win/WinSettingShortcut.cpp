#include "pch.h"
#include "WinSettingShortcut.h"
#include "WinSetting.h"
#include "Setting.h"
#include "Lang.h"
#include "Tray.h"

WinSettingShortcut::WinSettingShortcut(WinSetting* win):win{win}
{
	lineH *= win->dpi;
	inputW *= win->dpi;
	inputYMargin *= win->dpi;
	contentX = win->menuW + win->menuLeftSpan * 7;
	contentY = win->paddintTop + win->menuLeftSpan;
	contentR = win->w - win->menuLeftSpan * 7;
	auto w = contentR - contentX;
	capLabel = win->makeTextLayout(Lang::get(L"shortcut.cap"), w, lineH, win->textSize, false);
	longLabel = win->makeTextLayout(Lang::get(L"shortcut.long"), w, lineH, win->textSize, false);
	videoLabel = win->makeTextLayout(Lang::get(L"shortcut.video"), w, lineH, win->textSize, false);

	auto valSize{ 12 * win->dpi };
	auto configObj = Setting::getConfigObj();
	auto configShortcut = configObj.GetNamedObject(L"shortcutKey");
	std::wstring capKeyStr{ configShortcut.GetNamedString(L"cap") };
	capVal = win->makeTextLayout(capKeyStr, inputW, lineH, valSize);
	std::wstring longKeyStr{ configShortcut.GetNamedString(L"long") };
	longVal = win->makeTextLayout(longKeyStr, inputW, lineH, valSize);
	std::wstring videoKeyStr{ configShortcut.GetNamedString(L"video") };
	videoVal = win->makeTextLayout(videoKeyStr, inputW, lineH, valSize);
	tipVal = win->makeTextLayout(Lang::get(L"shortcut.pressKey"), inputW, lineH, valSize);
	win->render->CreateSolidColorBrush(D2D1::ColorF(0xFFFFFF), inputBg.GetAddressOf());
	win->render->CreateSolidColorBrush(D2D1::ColorF(0x1677FF), focusBrush.GetAddressOf());
}

WinSettingShortcut::~WinSettingShortcut()
{

}

void WinSettingShortcut::paint()
{
	win->render->DrawTextLayout({ contentX,contentY }, capLabel.Get(), win->textBrush2.Get());
	D2D1_ROUNDED_RECT rr = { { contentR - inputW, contentY + inputYMargin, contentR,contentY + lineH - inputYMargin }, win->borderRadius/2,win->borderRadius/2 };
	win->render->FillRoundedRectangle(rr, inputBg.Get());
	if (focusIndex == 0) {
		win->render->DrawRoundedRectangle(rr, focusBrush.Get(), win->dpi);
		win->render->DrawTextLayout({ rr.rect.left,contentY }, tipVal.Get(), win->textBrush2.Get());
	}
	else {
		win->render->DrawRoundedRectangle(rr, win->border.Get(), win->dpi);
		win->render->DrawTextLayout({ rr.rect.left,contentY }, capVal.Get(), win->textBrush2.Get());
	}


	auto y = contentY + lineH;
	win->render->DrawLine({ contentX,y }, { contentR,y }, win->border.Get(), win->dpi);
	win->render->DrawTextLayout({ contentX,y }, longLabel.Get(), win->textBrush2.Get());
	rr.rect.top += lineH;
	rr.rect.bottom += lineH;
	win->render->FillRoundedRectangle(rr, inputBg.Get());
	if (focusIndex == 1) {
		win->render->DrawRoundedRectangle(rr, focusBrush.Get(), win->dpi);
		win->render->DrawTextLayout({ rr.rect.left,y }, tipVal.Get(), win->textBrush2.Get());
	}
	else {
		win->render->DrawRoundedRectangle(rr, win->border.Get(), win->dpi);
		win->render->DrawTextLayout({ rr.rect.left,y }, longVal.Get(), win->textBrush2.Get());
	}

	y = y + lineH;
	win->render->DrawLine({ contentX,y }, { contentR,y }, win->border.Get(), win->dpi);
	win->render->DrawTextLayout({ contentX,y }, videoLabel.Get(), win->textBrush2.Get());
	rr.rect.top += lineH;
	rr.rect.bottom += lineH;
	win->render->FillRoundedRectangle(rr, inputBg.Get());	
	if (focusIndex == 2) {
		win->render->DrawRoundedRectangle(rr, focusBrush.Get(), win->dpi);
		win->render->DrawTextLayout({ rr.rect.left,y }, tipVal.Get(), win->textBrush2.Get());
	}
	else {
		win->render->DrawRoundedRectangle(rr, win->border.Get(), win->dpi);
		win->render->DrawTextLayout({ rr.rect.left,y }, videoVal.Get(), win->textBrush2.Get());
	}
	y = y + lineH;
	win->render->DrawLine({ contentX,y }, { contentR,y }, win->border.Get(), win->dpi);
}

void WinSettingShortcut::mouseMove(const int& x, const int& y)
{
	if (x > contentR - inputW && x<contentR) {
		if (y > contentY + inputYMargin && y < contentY + lineH - inputYMargin) {
			hoverIndex = 0;
		}
		else if (y > contentY + lineH + inputYMargin && y < contentY + lineH*2 - inputYMargin) {
			hoverIndex = 1;
		}
		else if (y > contentY + lineH*2 + inputYMargin && y < contentY + lineH * 3 - inputYMargin) {
			hoverIndex = 2;
		}
		else {
			hoverIndex = -1;
		}
	}
	else {
		hoverIndex = -1;
	}
}

void WinSettingShortcut::mouseDown()
{
	if (hoverIndex != focusIndex) {
		focusIndex = hoverIndex;
		tempKeys.clear();
		win->refresh();
	}
}

void WinSettingShortcut::keyDown(const std::wstring& key)
{
	if (key.empty()) return;
	bool isContains = std::find(tempKeys.begin(), tempKeys.end(), key) != tempKeys.end();
	if (isContains) return;
	tempKeys.push_back(key);
}

void WinSettingShortcut::keyUp()
{
	Setting::setKeys(focusIndex, tempKeys);
	if (auto tray = Tray::get()) {
		tray->reloadHotKeys();
	}
	auto valSize{ 12 * win->dpi };
	auto configObj = Setting::getConfigObj();
	auto configShortcut = configObj.GetNamedObject(L"shortcutKey");
	if (focusIndex == 0) {
		std::wstring str{ configShortcut.GetNamedString(L"cap") };
		capVal = win->makeTextLayout(str, inputW, lineH, valSize);
	}
	else if (focusIndex == 1) {
		std::wstring str{ configShortcut.GetNamedString(L"long") };
		longVal = win->makeTextLayout(str, inputW, lineH, valSize);
	}
	else if (focusIndex == 2) {
		std::wstring str{ configShortcut.GetNamedString(L"video") };
		videoVal = win->makeTextLayout(str, inputW, lineH, valSize);
	}
	focusIndex = -1;
	win->refresh();	
}
