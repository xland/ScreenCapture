#pragma once
#include "pch.h"
#include "WinBase.h"

class WinToolMain :public WinBase
{
public:
	WinToolMain(const int& x, const int& y, const int& w, const int& h);
	~WinToolMain();
	static void popup();
public:
protected:
	void onMouseDown(const int& x, const int& y, bool isRight) override;
private:
	void onPaint() override;
	bool onCursor() override;
	void initRes();
private:
	std::unordered_map<std::wstring, ComPtr<IDWriteTextLayout>> btnIcons;
	ComPtr<ID2D1SolidColorBrush> brushBg;
	ComPtr<ID2D1SolidColorBrush> brushIcon;
	ComPtr<ID2D1SolidColorBrush> brushIconHover;
	ComPtr<ID2D1SolidColorBrush> brushSpliter;
	int hoverIndex{ -1 };
	float btnW{ 36.f }, btnH{ 32.f }, btnPadding;
};

