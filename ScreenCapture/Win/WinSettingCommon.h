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
public:
	int hoverIndex{ -1 };
private:
	WinSetting* win;
	float lineH{ 44.f }, iconSize{ 22.f };
	float contentX, contentY, contentR;
	bool isAutoStart{ false }, isShowTray{ true };
	ComPtr<IDWriteTextLayout> startupLabel, trayLabel, langLabel;
	ComPtr<IDWriteTextLayout> closeIcon, openIcon;
	ComPtr<ID2D1SolidColorBrush> openBrush;
};

