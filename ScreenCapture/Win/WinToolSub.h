#pragma once
#include "pch.h"
#include "WinToolBase.h"
class WinToolSub :public WinToolBase
{
public:
	WinToolSub();
	~WinToolSub();
	static void popup();
	static WinToolSub* get();
	D2D1_COLOR_F getSelectedColor();
public:
	float sliderVal;
protected:
	void onPaint() override;
private:
	bool onCursor() override;
	void initBorder();
	void initColor();
	void onMouseMove(const int& x, const int& y) override;
	void onMouseLeave() override;
	void onMouseDown(const int& x, const int& y, bool isRight) override;
	void onMouseDrag(const int& x, const int& y, const UINT_PTR& modifiers) override;
	void onMouseWheel(const int& x, const int& y, const short& delta) override;
	void paintSlider();
	void paintColorSelector();
	void initVal();
	void initBtn();
	bool hoverBtn(const int& x);
	bool hoverColor(const int& x);
private:
	float arrowX, btnStart{0.f}, btnEnd,sliderStart,sliderEnd,colorStart,colorEnd, colorBtnW, sliderMin,sliderMax;
	ComPtr<ID2D1PathGeometry> borderPath;
	std::vector<ComPtr<ID2D1SolidColorBrush>> colorBrush;
	std::vector<std::wstring> colorName;
	int hoverColorIndex{ -1 }, selectColorIndex{0};
};

