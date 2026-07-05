#include "pch.h"
#include "App.h"
#include "WinSetting.h"
#include "WinSettingCommon.h"
#include "WinSettingAbout.h"
#include "WinSettingShortcut.h"
#include "Lang.h"
#include "Util.h"

std::unique_ptr<WinSetting> winSetting;

WinSetting::WinSetting(const int& x, const int& y, const int& w, const int& h) :WinBase(x, y, w, h)
{
	
}

WinSetting::~WinSetting()
{

}

void WinSetting::init()
{
	if (winSetting.get()) {
		if (IsIconic(winSetting->hwnd)) {
			ShowWindow(winSetting->hwnd, SW_RESTORE);
		}
		else {
			winSetting->show();
		}
		SetForegroundWindow(winSetting->hwnd);
	}
	else {
		App::initDevice();
		RECT workAreaRect;
		BOOL getWorkAreaSuccess = SystemParametersInfo(SPI_GETWORKAREA, 0, &workAreaRect, 0); //工作区矩形
		int workAreaWidth = workAreaRect.right - workAreaRect.left;
		int workAreaHeight = workAreaRect.bottom - workAreaRect.top;
		UINT dpiRaw = GetDpiForSystem();
		float dpi = dpiRaw / 96.0f;
		int w = 1100;
		int h = 780;
		int x = workAreaRect.left + (workAreaWidth - w) / 2;
		int y = workAreaRect.top + (workAreaHeight - h) / 2;
		winSetting = std::make_unique<WinSetting>(x, y, w, h);
		winSetting->createWindow(WS_EX_APPWINDOW, WS_MAXIMIZEBOX | WS_MINIMIZEBOX);
	}
}

void WinSetting::onCreated()
{
	SetWindowText(hwnd, Lang::get(L"setting.title").c_str());
	enableShadow();
	menuW *= dpi;
	menuH *= dpi;
	paddintTop *= dpi;
	menuLeftSpan *= dpi;
	borderRadius *= dpi;
	textSize *= dpi;

	render->CreateSolidColorBrush(D2D1::ColorF(0xE3E3E5), bg.GetAddressOf());
	render->CreateSolidColorBrush(D2D1::ColorF(0xEEEEF0), menuBg.GetAddressOf());
	render->CreateSolidColorBrush(D2D1::ColorF(0xD6D6D8), border.GetAddressOf());
	render->CreateSolidColorBrush(D2D1::ColorF(0x888888), textBrush.GetAddressOf());
	render->CreateSolidColorBrush(D2D1::ColorF(0xFA5151), red.GetAddressOf());
	render->CreateSolidColorBrush(D2D1::ColorF(0x333333), textBrush2.GetAddressOf()); //0x1677FF
	menuLabels.push_back(makeTextLayout(Lang::get(L"setting.common"), menuW, menuH, textSize));
	menuLabels.push_back(makeTextLayout(Lang::get(L"setting.shortcut"), menuW, menuH, textSize));
	menuLabels.push_back(makeTextLayout(Lang::get(L"setting.about"), menuW, menuH, textSize));
	closeIcon = makeIconLayout(L"\ue62d", paddintTop, paddintTop, textSize);
	common = std::make_unique<WinSettingCommon>(this);
	shortcut = std::make_unique<WinSettingShortcut>(this);
	about = std::make_unique<WinSettingAbout>(this);

	show();
}

void WinSetting::onPaint()
{
	render->Clear(D2D1::ColorF(0xE3E3E5));
	render->DrawTextLayout({ w-paddintTop,0 }, closeIcon.Get(), indexHover == 3? red.Get() : textBrush.Get());

	D2D1_RECT_F menuRect{ 0,0,menuW,h };
	render->FillRectangle(menuRect, menuBg.Get());
	render->DrawLine({ menuW,0 }, { menuW,(float)h }, border.Get(), dpi);
	for (size_t i = 0; i < menuLabels.size(); i++)
	{
		auto y{ paddintTop + menuH * i };
		if (i == menuIndexSelect) {
			D2D1_ROUNDED_RECT rr = { { menuLeftSpan, y, menuW + menuLeftSpan, menuH+y }, borderRadius,borderRadius };
			render->FillRoundedRectangle(rr, bg.Get());
			render->DrawRoundedRectangle(rr, border.Get(), dpi);
			render->FillRectangle({ menuW, y-dpi, menuW + menuLeftSpan+dpi, menuH + y+dpi }, bg.Get());
			render->DrawTextLayout({ 0,y }, menuLabels[i].Get(), textBrush2.Get());
		}
		else {
			render->DrawTextLayout({ 0,y }, menuLabels[i].Get(), i == indexHover ? textBrush2.Get() : textBrush.Get());
		}		
	}
	D2D1_ROUNDED_RECT rr = { { menuW + menuLeftSpan*4,paddintTop, w - menuLeftSpan*4, h- menuLeftSpan*4 }, borderRadius,borderRadius };
	render->FillRoundedRectangle(rr, menuBg.Get());

	if (menuIndexSelect == 0) {
		common->paint();
	}
	else if (menuIndexSelect == 1) {
		shortcut->paint();
	}
	else {
		about->paint();
	}
}

BOOL WinSetting::onCursor()
{
	if (indexHover >= 0 || common->hoverIndex>=0 || about->hoverIndex>=0) {
		setCursor(IDC_HAND);
	}
	else if (shortcut->hoverIndex>=0) {
		setCursor(IDC_IBEAM);
	}
	else {
		setCursor(IDC_ARROW);
	}	
	return TRUE;
}

void WinSetting::onMouseDown(const int& x, const int& y, bool isRight)
{
	if (indexHover < 0 && common->hoverIndex<0 && about->hoverIndex < 0 && shortcut->hoverIndex < 0) {
		pressPos.x = x;
		pressPos.y = y;
		shortcut->mouseDown();
		return;
	}
	if (indexHover != menuIndexSelect && indexHover >=0 && indexHover<=2) {
		if (common) common->collapseLang();
		menuIndexSelect = indexHover;
		refresh();
		return;
	}
	if (indexHover == 3) {
		close();
		winSetting.reset();
		App::disposeDeviceIfIdle();
		return;
	}
	common->mouseDown();
	shortcut->mouseDown();
	about->mouseDown();
}

void WinSetting::onMouseDrag(const int& x, const int& y, const UINT_PTR& modifiers)
{
	if (indexHover < 0 && common->hoverIndex < 0 && about->hoverIndex < 0) {
		this->x += (x - pressPos.x);
		this->y += (y - pressPos.y);
		move(this->x, this->y);
	}
}

void WinSetting::onMouseMove(const int& x, const int& y)
{
	int index{ -1 };
	if (x > menuLeftSpan && x < menuW && y>paddintTop) {
		int i = (y-paddintTop) / menuH;
		if (i <= 2&& i>=0) {
			index = i;
		}
	}
	else if (y > 0 && y<paddintTop && x>w - paddintTop) {
		index = 3;
	}
	if (menuIndexSelect == 0) {
		common->mouseMove(x, y);
	}
	else if (menuIndexSelect == 1) {
		shortcut->mouseMove(x, y);
	}
	else if (menuIndexSelect == 2) {
		about->mouseMove(x, y);
	}

	if (index != indexHover) {
		indexHover = index;
		refresh();
	}
}

void WinSetting::onMouseLeave()
{
	if (indexHover != -1) {
		indexHover = -1;
		refresh();
	}
}

void WinSetting::onDpiChanged()
{
	// 用头文件里的基准值按新 dpi 重新缩放（原字段已被 onCreated 就地乘过 dpi，这里直接重置）
	menuW = 120.f * dpi;
	menuH = 38.f * dpi;
	paddintTop = 38.f * dpi;
	menuLeftSpan = 8.f * dpi;
	borderRadius = 6.f * dpi;
	textSize = 14.f * dpi;

	// 依赖上述尺寸/字号的文本与图标图层需要按新度量重建
	menuLabels.clear();
	menuLabels.push_back(makeTextLayout(Lang::get(L"setting.common"), menuW, menuH, textSize));
	menuLabels.push_back(makeTextLayout(Lang::get(L"setting.shortcut"), menuW, menuH, textSize));
	menuLabels.push_back(makeTextLayout(Lang::get(L"setting.about"), menuW, menuH, textSize));
	closeIcon = makeIconLayout(L"\ue62d", paddintTop, paddintTop, textSize);

	// 子面板在构造函数里根据 win->dpi、menuW、paddintTop、menuLeftSpan、w 计算自身的
	// contentX/contentY/contentR 与所有内部尺寸，整体重建最简单且状态不丢（配置项来自持久化设置）
	common = std::make_unique<WinSettingCommon>(this);
	shortcut = std::make_unique<WinSettingShortcut>(this);
	about = std::make_unique<WinSettingAbout>(this);

	// 悬停状态在新的坐标系里已失效
	indexHover = -1;
}

void WinSetting::onKeyDown(const UINT& key)
{
	if (shortcut->focusIndex >=0) {
		if (GetKeyState(VK_CONTROL) & 0x8000) {
			shortcut->keyDown(L"Ctrl");
		}
		if (GetKeyState(VK_MENU) & 0x8000) {
			shortcut->keyDown(L"Alt");
		}
		if (GetKeyState(VK_SHIFT) & 0x8000) {
			shortcut->keyDown(L"Shift");
		}
		if (GetAsyncKeyState(VK_LWIN) & 0x8000) {
			shortcut->keyDown(L"LWin");
		}
		if (GetAsyncKeyState(VK_RWIN) & 0x8000) {
			shortcut->keyDown(L"RWin");
		}
		shortcut->keyDown(Util::keyToStr(key));
	}
}

void WinSetting::onKeyUp()
{
	if (shortcut->focusIndex >= 0) {
		shortcut->keyUp();
	}
}
