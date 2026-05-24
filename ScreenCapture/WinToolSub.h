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
	void buildBorderPath();
	static void getXW(int& x,int& w,float& arrowX);
	void onMouseMove(const int& x, const int& y) override;
	void onMouseLeave() override;
	void onMouseDown(const int& x, const int& y, bool isRight) override;
	void paintSlider();
	void paintColorSelector();
private:
	float arrowX, posX{0.f};
	ComPtr<ID2D1PathGeometry> borderPath;
};

