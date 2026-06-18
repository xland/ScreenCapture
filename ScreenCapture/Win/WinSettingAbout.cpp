#include "pch.h"
#include "Util.h"
#include "WinSettingAbout.h"
#include "WinSetting.h"

WinSettingAbout::WinSettingAbout(WinSetting* win):win{win}
{
	lineH *= win->dpi;
	contentX = win->menuW + win->menuLeftSpan * 7;
	contentY = win->paddintTop + win->menuLeftSpan;
	contentR = win->w - win->menuLeftSpan * 7;
	auto w = contentR - contentX;
	win->render->CreateSolidColorBrush(D2D1::ColorF(0x1677FF), linkBrush.GetAddressOf());
	verLabel = win->makeTextLayout(L"版本：", w, lineH, win->textSize, false);
	verVal = win->makeTextLayout(L"2.4.0", w, lineH, win->textSize, false);
	verVal->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
	linkLabel = win->makeTextLayout(L"项目：", w, lineH, win->textSize, false);
	linkVal = win->makeTextLayout(L"github.com/xland/ScreenCapture", w, lineH, win->textSize, false);
	linkVal->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
	authorLabel = win->makeTextLayout(L"作者：", w, lineH, win->textSize, false);
	authorVal = win->makeTextLayout(L"微信：liulun_007（点击复制）", w, lineH, win->textSize, false);
	authorVal->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
}

WinSettingAbout::~WinSettingAbout()
{

}

void WinSettingAbout::paint()
{
	win->render->DrawTextLayout({ contentX,contentY }, verLabel.Get(), win->textBrush2.Get());
	win->render->DrawTextLayout({ contentX,contentY }, verVal.Get(), win->textBrush2.Get());
	auto y = contentY + lineH;
	win->render->DrawLine({ contentX,y }, { contentR,y }, win->border.Get(), win->dpi);
	win->render->DrawTextLayout({ contentX,y }, linkLabel.Get(), win->textBrush2.Get());
	win->render->DrawTextLayout({ contentX,y }, linkVal.Get(), linkBrush.Get());
	y = y + lineH;
	win->render->DrawLine({ contentX,y }, { contentR,y }, win->border.Get(), win->dpi);
	win->render->DrawTextLayout({ contentX,y }, authorLabel.Get(), win->textBrush2.Get());
	win->render->DrawTextLayout({ contentX,y }, authorVal.Get(), win->textBrush2.Get());
	y = y + lineH;
	win->render->DrawLine({ contentX,y }, { contentR,y }, win->border.Get(), win->dpi);	
}
void WinSettingAbout::mouseMove(const int& x, const int& y)
{
	if (x > contentX && x<contentR && y>contentY) {
		if (y> contentY + lineH && y < contentY + lineH*2) {
			hoverIndex = 0;
		}
		else if (y > contentY + 2 * lineH && y < contentY + 3 * lineH) {
			hoverIndex = 1;
		}
		else {
			hoverIndex = -1;
		}
	}
	else {
		hoverIndex = -1;
	}
}

void WinSettingAbout::mouseDown()
{
	if (hoverIndex == 0) {
		std::wstring downloadUrl{ L"https://github.com/xland/ScreenCapture" };
		ShellExecute(win->hwnd, L"open", downloadUrl.data(), nullptr, nullptr, SW_SHOWNORMAL);
		hoverIndex = -1;
		win->isMouseDown = false;
	}
	else if (hoverIndex == 1) {
		Util::setTextToClipboard(L"liulun_007");
		MessageBox(win->hwnd, L"复制成功", L"系统提示", MB_OK | MB_ICONINFORMATION);
		hoverIndex = -1;
		win->isMouseDown = false;
	}
}