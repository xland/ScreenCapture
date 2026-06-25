#pragma once
#include "pch.h"
class WinSetting;
class WinSettingCommon
{
public:
	WinSettingCommon(WinSetting* win);
	~WinSettingCommon();
	void paint();
	void mouseMove(const int& x, const int& y);
	void mouseDown();
	/// 收起语言下拉框（切换标签页/失焦时由 WinSetting 调用）
	void collapseLang();
public:
	int hoverIndex{ -1 };
private:
	WinSetting* win;
	float lineH{ 44.f }, iconSize{ 22.f };
	float contentX, contentY, contentR;
	bool isAutoStart{ false }, isShowTray{ true };
	// 语言下拉框（combobox）
	bool langExpanded{ false };
	int langHover{ -1 };
	float langBoxW{ 200.f }, langItemH{ 34.f };
	ComPtr<IDWriteTextLayout> startupLabel, trayLabel, langLabel;
	ComPtr<IDWriteTextLayout> langVal, langArrow;
	std::vector<ComPtr<IDWriteTextLayout>> langItemLayouts;
	ComPtr<IDWriteTextLayout> closeIcon, openIcon;
	ComPtr<ID2D1SolidColorBrush> openBrush, langBoxBg, langHoverBg;
private:
	void applyLanguage();
	void buildLangItems();
	void updateLangVal();
	void selectLang(const int& index);
	int currentLangIndex() const;
};
