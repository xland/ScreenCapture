#pragma once
#include "pch.h"
#include "Win/WinBase.h"
class WinPin;
class ToolBase :public WinBase
{
public:
	ToolBase(const int& x, const int& y, const int& w, const int& h);
	virtual ~ToolBase();
	void initBrush();
	void initTip();
public:
	int selectIndex{-1};
protected:
	void paintIcon(const float& posX,IDWriteTextLayout* icon,bool isHover,bool isSelected);
	IDWriteTextLayout* getBtnIconLayout(const std::wstring& name);
	void showTipAt(int x, int y);
	void hideTip();
protected:
	ComPtr<ID2D1SolidColorBrush> brushBg;
	ComPtr<ID2D1SolidColorBrush> brushSelect;
	ComPtr<ID2D1SolidColorBrush> brushIcon;
	ComPtr<ID2D1SolidColorBrush> brushBlue;
	ComPtr<ID2D1SolidColorBrush> brushSpliter;
	std::vector<std::wstring> btnId;
	std::vector<ComPtr<IDWriteTextLayout>> btnLayout;
	int hoverIndex{ -1 };
	float marginTop,btnSize;
	HWND tipHwnd;
	std::wstring tipText;
private:
private:
	TOOLINFO ti{ 0 };
};

