#pragma once
#include "pch.h"
#include "Win/WinBase.h"
class WinPin;
class WinToolBase :public WinBase
{
public:
	WinToolBase(const int& x, const int& y, const int& w, const int& h, WinPin* parent);
	virtual ~WinToolBase();
	void initBrush();
	void initTip();
public:
	int selectIndex{-1};
protected:
	void addIconLayout(const std::wstring& icon);
	void paintIcon(const float& posX,IDWriteTextLayout* icon,bool isHover,bool isSelected);
	IDWriteTextLayout* getBtnIconLayout(const std::string& name);
	void onKeyDown(const UINT& key) override;
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
	float marginTop,btnSize;
	HWND tipHwnd;
	std::wstring tipText;
	WinPin* parent;
private:
private:
	TOOLINFO ti{ 0 };
};

