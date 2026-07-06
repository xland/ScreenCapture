#include "pch.h"
#include "WinSettingCommon.h"
#include "WinSetting.h"
#include "WinSettingShortcut.h"
#include "WinSettingAbout.h"
#include "Setting.h"
#include "App.h"
#include "Lang.h"
#include "Util.h"
#include "Tray.h"

WinSettingCommon::WinSettingCommon(WinSetting* win):win{win}
{
	iconSize *= win->dpi;
	lineH *= win->dpi;
	langBoxW *= win->dpi;
	langItemH *= win->dpi;
	contentX = win->menuW + win->menuLeftSpan * 7;
	contentY = win->paddintTop + win->menuLeftSpan;
	contentR = win->w - win->menuLeftSpan * 7;

	auto configObj = Setting::getConfigObj();
	auto common = configObj.GetNamedObject(L"common");
	isShowTray = common.GetNamedBoolean(L"showTray");
	isAutoStart = common.GetNamedBoolean(L"autoStart");

	win->render->CreateSolidColorBrush(D2D1::ColorF(0x1677FF), openBrush.GetAddressOf());
	win->render->CreateSolidColorBrush(D2D1::ColorF(0xFFFFFF), langBoxBg.GetAddressOf());
	win->render->CreateSolidColorBrush(D2D1::ColorF(0xE6F4FF), langHoverBg.GetAddressOf());
	auto w = contentR - contentX;
	startupLabel = App::makeTextLayout(Lang::get(L"setting.autoStart"), w, lineH, win->textSize, false);
	trayLabel = App::makeTextLayout(Lang::get(L"setting.showTray"), w, lineH, win->textSize, false);
	langLabel = App::makeTextLayout(Lang::get(L"setting.language"), w, lineH, win->textSize, false);


	w = langBoxW - 24.f * win->dpi;
	for (const auto& info : Lang::getLangs()) {
		langItemLayouts.push_back(App::makeTextLayout(info.second, w, langItemH, win->textSize, false));
	}

	updateLangVal();
	langArrow = App::makeTextLayout(L"▾", 22.f * win->dpi, lineH, iconSize);
	closeIcon = App::makeIconLayout(L"\ue687", lineH, lineH, iconSize);
	openIcon = App::makeIconLayout(L"\ue688", lineH, lineH, iconSize);
}

WinSettingCommon::~WinSettingCommon()
{

}

void WinSettingCommon::paint()
{
	// 行0：开机自启动
	win->render->DrawTextLayout({ contentX,contentY }, startupLabel.Get(), win->textBrush2.Get());
	if (isAutoStart) {
		win->render->DrawTextLayout({ contentR - lineH,contentY }, openIcon.Get(), openBrush.Get());
	}
	else {
		win->render->DrawTextLayout({ contentR - lineH,contentY }, closeIcon.Get(), win->textBrush.Get());
	}
	// 行1：托盘图标
	auto y = contentY + lineH;
	win->render->DrawLine({ contentX,y }, { contentR,y }, win->border.Get(), win->dpi);
	win->render->DrawTextLayout({ contentX,y }, trayLabel.Get(), win->textBrush2.Get());
	if (isShowTray) {
		win->render->DrawTextLayout({ contentR - lineH,y }, openIcon.Get(), openBrush.Get());
	}
	else {
		win->render->DrawTextLayout({ contentR - lineH,y }, closeIcon.Get(), win->textBrush.Get());
	}
	// 行2：语言（combobox）
	y += lineH;
	win->render->DrawLine({ contentX,y }, { contentR,y }, win->border.Get(), win->dpi);
	win->render->DrawTextLayout({ contentX,y }, langLabel.Get(), win->textBrush2.Get());
	float boxX = contentR - langBoxW;
	float padY = 8.f * win->dpi;
	float boxTop = y + padY;
	float boxBottom = y + lineH - padY;
	float radius = 4.f * win->dpi;
	D2D1_RECT_F box = { boxX, boxTop, contentR, boxBottom };
	win->render->FillRectangle(box, langBoxBg.Get());
	bool boxHot = (hoverIndex == 2) || langExpanded;
	win->render->DrawRectangle(box, boxHot ? openBrush.Get() : win->border.Get(), win->dpi);
	win->render->DrawTextLayout({ boxX + 10.f * win->dpi, y }, langVal.Get(), win->textBrush2.Get());
	win->render->DrawTextLayout({ contentR - 22.f * win->dpi, y }, langArrow.Get(), win->textBrush.Get());
	// 行2 底部分隔线
	win->render->DrawLine({ contentX,y + lineH }, { contentR,y + lineH }, win->border.Get(), win->dpi);
	// 展开的下拉列表（绘制在最上层，盖住下方分隔线）
	if (langExpanded) {
		float listH = (float)langItemLayouts.size() * langItemH;
		D2D1_RECT_F listRect = { boxX, boxBottom, contentR, boxBottom + listH };
		win->render->FillRectangle(listRect, langBoxBg.Get());
		int cur = currentLangIndex();
		float padX = 10.f * win->dpi;
		for (int i = 0; i < (int)langItemLayouts.size(); i++) {
			float iy = boxBottom + i * langItemH;
			if (i == langHover) {
				D2D1_ROUNDED_RECT ir = { { boxX, iy, contentR, iy + langItemH }, radius, radius };
				win->render->FillRoundedRectangle(ir, langHoverBg.Get());
			}
			auto brush = (i == cur) ? openBrush.Get() : win->textBrush2.Get();
			win->render->DrawTextLayout({ boxX + padX, iy }, langItemLayouts[i].Get(), brush);
		}
		win->render->DrawRectangle(listRect, openBrush.Get(), win->dpi);
	}
}

void WinSettingCommon::mouseMove(const int& x, const int& y)
{
	if (langExpanded) {
		float boxX = contentR - langBoxW;
		float boxBottom = contentY + 3.f * lineH - 5.f * win->dpi;
		int newHover = -1;
		if (x >= boxX && x <= contentR && y >= boxBottom) {
			int idx = (int)((y - boxBottom) / langItemH);
			if (idx >= 0 && idx < (int)langItemLayouts.size()) newHover = idx;
		}
		bool changed = (newHover != langHover);
		langHover = newHover;
		// 展开时保持 hoverIndex=2：维持手型光标，并让 WinSetting 不进入拖动分支，
		// 从而鼠标按下一定能落到 common->mouseDown（选择项或收起）。
		hoverIndex = 2;
		if (changed) win->refresh();
		return;
	}
	hoverIndex = -1;
	if (x > contentR - lineH && x<contentR && y>contentY) {
		if (y < contentY + lineH) hoverIndex = 0;
		else if (y < contentY + 2 * lineH) hoverIndex = 1;
	}
	else if (x > contentR - langBoxW && x < contentR && y > contentY + 2 * lineH && y < contentY + 3 * lineH) {
		hoverIndex = 2;
	}
}

void WinSettingCommon::mouseDown()
{
	if (langExpanded) {
		if (langHover >= 0 && langHover < (int)langItemLayouts.size()) {
			selectLang(langHover);
		}
		else {
			langExpanded = false;
		}
		langHover = -1;
		win->refresh();
		return;
	}
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
	else if (hoverIndex == 2) {
		langExpanded = true;
		langHover = -1;
		win->refresh();
	}
}

void WinSettingCommon::collapseLang()
{
	if (langExpanded) {
		langExpanded = false;
		langHover = -1;
		win->refresh();
	}
}

void WinSettingCommon::applyLanguage()
{
	auto w = contentR - contentX;
	startupLabel = App::makeTextLayout(Lang::get(L"setting.autoStart"), w, lineH, win->textSize, false);
	trayLabel = App::makeTextLayout(Lang::get(L"setting.showTray"), w, lineH, win->textSize, false);
	langLabel = App::makeTextLayout(Lang::get(L"setting.language"), w, lineH, win->textSize, false);
	updateLangVal();
	// 重建兄弟面板与菜单（WinSettingCommon 是 WinSetting 的友元，安全：不重建自身 this）
	SetWindowText(win->hwnd, Lang::get(L"setting.title").c_str());
	win->menuLabels.clear();
	win->menuLabels.push_back(App::makeTextLayout(Lang::get(L"setting.common"), win->menuW, win->menuH, win->textSize));
	win->menuLabels.push_back(App::makeTextLayout(Lang::get(L"setting.shortcut"), win->menuW, win->menuH, win->textSize));
	win->menuLabels.push_back(App::makeTextLayout(Lang::get(L"setting.about"), win->menuW, win->menuH, win->textSize));
	win->shortcut = std::make_unique<WinSettingShortcut>(win);
	win->about = std::make_unique<WinSettingAbout>(win);
}

void WinSettingCommon::updateLangVal()
{
	float w = langBoxW - 34.f * win->dpi; // 右侧留出箭头空间
	auto name = Lang::getLangs().at(Lang::getLang());
	langVal = App::makeTextLayout(name, w, lineH, win->textSize, false);
}

void WinSettingCommon::selectLang(const int& index)
{
	const auto& langs = Lang::getLangs();
	langExpanded = false;
	if (index < 0 || index >= (int)langs.size()) return;

	std::wstring code;
	int i = 0;
	for (const auto& info : Lang::getLangs()) {
		if (i == index) {
			code = info.first;
			break;
		}
		i += 1;
	}

	if (code == Lang::getLang()) return; // 无变化
	Setting::setLanguage(code);
	Lang::setLang(code);
	applyLanguage();
}

int WinSettingCommon::currentLangIndex() const
{
	auto cur = Lang::getLang();
	int i = 0;
	for (const auto& info : Lang::getLangs()) {
		if (info.first == cur) return i;
		i++;
	}
	return 0;
}
