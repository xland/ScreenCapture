#pragma once
#include "pch.h"
#include "WinBase.h"
class WinToolBase :public WinBase
{
public:
	WinToolBase(const int& x, const int& y, const int& w, const int& h);
	~WinToolBase();
public:
	int selectIndex{0 };
protected:
	void initBrush();
	void addBtns(const std::vector<std::pair<std::wstring, std::wstring>>& btns);
protected:
	ComPtr<ID2D1SolidColorBrush> brushBg;
	ComPtr<ID2D1SolidColorBrush> brushSelect;
	ComPtr<ID2D1SolidColorBrush> brushIcon;
	ComPtr<ID2D1SolidColorBrush> brushBlue;
	ComPtr<ID2D1SolidColorBrush> brushSpliter;
	std::vector<std::pair<std::wstring, ComPtr<IDWriteTextLayout>>> btnIcons;
	int hoverIndex{ -1 };
};

