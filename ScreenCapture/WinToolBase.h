#pragma once
#include "pch.h"
#include "WinBase.h"
class WinToolBase :public WinBase
{
public:
	WinToolBase(const int& x, const int& y, const int& w, const int& h);
	~WinToolBase();
public:
	int selectIndex{-1};
protected:
	void initBrush();
	void paintIcon(const float& posX,IDWriteTextLayout* icon,bool isHover,bool isSelected);
	void addBtns(const std::vector<std::pair<std::wstring, std::wstring>>& btns);
	IDWriteTextLayout* getBtnIconLayout(const std::wstring& name);
protected:
	ComPtr<ID2D1SolidColorBrush> brushBg;
	ComPtr<ID2D1SolidColorBrush> brushSelect;
	ComPtr<ID2D1SolidColorBrush> brushIcon;
	ComPtr<ID2D1SolidColorBrush> brushBlue;
	ComPtr<ID2D1SolidColorBrush> brushSpliter;
	std::vector<std::pair<std::wstring, ComPtr<IDWriteTextLayout>>> btnIcons;
	int hoverIndex{ -1 };
	float btnSize, marginTop;
};

