#include "pch.h"
#include "Util.h"
#include "App.h"
#include "Lang.h"
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
	verLabel = App::makeTextLayout(Lang::get(L"about.version"), w, lineH, win->textSize, false);
	verLabel->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
	verVal = App::makeTextLayout(Util::getVer(), w, lineH, win->textSize, false);
	verVal->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
	verVal->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
	linkLabel = App::makeTextLayout(Lang::get(L"about.project"), w, lineH, win->textSize, false);
	linkLabel->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
	linkVal = App::makeTextLayout(L"github.com/xland/ScreenCapture", w, lineH, win->textSize, false);
	linkVal->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
	linkVal->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
	authorLabel = App::makeTextLayout(Lang::get(L"about.author"), w, lineH, win->textSize, false);
	authorLabel->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
	authorVal = App::makeTextLayout(Lang::get(L"about.wechat"), w, lineH, win->textSize, false);
	authorVal->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
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
	if (x > contentX && x<contentR ) {
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
		MessageBox(win->hwnd, Lang::get(L"about.copySuccess").c_str(), Lang::get(L"about.sysTip").c_str(), MB_OK | MB_ICONINFORMATION);
		hoverIndex = -1;
		win->isMouseDown = false;
	}
}