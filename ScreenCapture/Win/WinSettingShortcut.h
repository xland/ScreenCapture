#pragma once
#include "pch.h"
class WinSetting;
class WinSettingShortcut
{
public:
	WinSettingShortcut(WinSetting* win);
	~WinSettingShortcut();
	void paint();
private:
	WinSetting* win;
	float lineH{ 44.f }, iconSize{ 22.f };
	float contentX, contentY, contentR;
	bool isAutoStart{ false }, isShowTray{ true };
	ComPtr<IDWriteTextLayout> startupLabel, trayLabel, langLabel;
	ComPtr<IDWriteTextLayout> closeIcon, openIcon;
	ComPtr<ID2D1SolidColorBrush> openBrush;
};

