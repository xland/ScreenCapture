#pragma once
#include "pch.h"
class WinSetting;
class WinSettingAbout
{
public:
	WinSettingAbout(WinSetting* win);
	~WinSettingAbout();
	void paint();
private:
	WinSetting* win;
	float lineH{ 44.f };
	float contentX, contentY, contentR;
	ComPtr<ID2D1SolidColorBrush> linkBrush;
	ComPtr<IDWriteTextLayout> verLabel, verVal, linkLabel, linkVal, authorLabel, authorVal;
};

