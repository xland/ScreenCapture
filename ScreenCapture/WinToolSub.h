#pragma once
#include "pch.h"
#include "WinToolBase.h"
class WinToolSub :public WinToolBase
{
public:
	WinToolSub(const int& x, const int& y, const int& w, const int& h);
	~WinToolSub();
	static void popup();
	static WinToolSub* get();
	void changeState();

public:
protected:
	void onPaint() override;
private:
	bool onCursor() override;
	void initBorder();
	void initColor();
	static void getXW(int& x,int& w,float& arrowX);
	void onMouseMove(const int& x, const int& y) override;
	void onMouseLeave() override;
	void onMouseDown(const int& x, const int& y, bool isRight) override;
	void paintSlider();
	void paintColorSelector();
private:
	float arrowX, btnStart{0.f}, btnEnd,sliderStart,sliderEnd,colorStart,colorEnd, colorBtnW;
	ComPtr<ID2D1PathGeometry> borderPath;
	std::vector<ComPtr<ID2D1SolidColorBrush>> colorBrush;
	int hoverColorIndex{ -1 }, selectColorIndex{0};

};

