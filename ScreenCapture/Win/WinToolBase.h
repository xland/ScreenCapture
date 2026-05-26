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
	void initTip();
	void paintIcon(const float& posX,IDWriteTextLayout* icon,bool isHover,bool isSelected);
	IDWriteTextLayout* getBtnIconLayout(const std::string& name);
	void showTipAt(int x, int y);
	void hideTip();
protected:
	ComPtr<ID2D1SolidColorBrush> brushBg;
	ComPtr<ID2D1SolidColorBrush> brushSelect;
	ComPtr<ID2D1SolidColorBrush> brushIcon;
	ComPtr<ID2D1SolidColorBrush> brushBlue;
	ComPtr<ID2D1SolidColorBrush> brushSpliter;
	std::vector<std::string> btnId;
	std::vector<std::wstring> btnName;
	std::vector<ComPtr<IDWriteTextLayout>> btnLayout;
	int hoverIndex{ -1 };
	float btnSize, marginTop;
	HWND tipHwnd;
	std::wstring tipText;
private:
private:
	TOOLINFO ti{ 0 };
};

