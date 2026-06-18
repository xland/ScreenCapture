#include "pch.h"
#include "WinSettingShortcut.h"
#include "WinSetting.h"

WinSettingShortcut::WinSettingShortcut(WinSetting* win):win{win}
{
	lineH *= win->dpi;
	contentX = win->menuW + win->menuLeftSpan * 7;
	contentY = win->paddintTop + win->menuLeftSpan;
	contentR = win->w - win->menuLeftSpan * 7;
	auto w = contentR - contentX;
	capLabel = win->makeTextLayout(L"屏幕截图：", w, lineH, win->textSize, false);
	longLabel = win->makeTextLayout(L"滚动截图：", w, lineH, win->textSize, false);
	videoLabel = win->makeTextLayout(L"屏幕录制：", w, lineH, win->textSize, false);
}

WinSettingShortcut::~WinSettingShortcut()
{

}

void WinSettingShortcut::paint()
{
	win->render->DrawTextLayout({ contentX,contentY }, capLabel.Get(), win->textBrush2.Get());
	auto y = contentY + lineH;
	win->render->DrawLine({ contentX,y }, { contentR,y }, win->border.Get(), win->dpi);
	win->render->DrawTextLayout({ contentX,y }, longLabel.Get(), win->textBrush2.Get());
	y = y + lineH;
	win->render->DrawLine({ contentX,y }, { contentR,y }, win->border.Get(), win->dpi);
	win->render->DrawTextLayout({ contentX,y }, videoLabel.Get(), win->textBrush2.Get());
	y = y + lineH;
	win->render->DrawLine({ contentX,y }, { contentR,y }, win->border.Get(), win->dpi);
}

void WinSettingShortcut::mouseMove(const int& x, const int& y)
{
}

void WinSettingShortcut::mouseDown()
{
}
