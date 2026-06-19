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
	int focusIndex{ -1 }, hoverIndex{ -1 };
private:
	WinSetting* win;
	float lineH{ 44.f }, inputW{ 96.f }, inputYMargin{8.f};
	float contentX, contentY, contentR;
	ComPtr<IDWriteTextLayout> capLabel, longLabel, videoLabel, capVal, longVal, videoVal,tipVal;
	ComPtr<ID2D1SolidColorBrush> inputBg,focusBrush;
};

