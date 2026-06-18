#pragma once
#include "pch.h"
class WinSetting;
class WinSettingShortcut
{
public:
	WinSettingShortcut(WinSetting* win);
	~WinSettingShortcut();
	void paint();	
	void mouseMove(const int& x, const int& y);
	void mouseDown();
private:
	WinSetting* win;
	float lineH{ 44.f }, iconSize{ 22.f };
	float contentX, contentY, contentR;
	bool isAutoStart{ false }, isShowTray{ true };
	ComPtr<IDWriteTextLayout> capLabel, longLabel, videoLabel;
};

