#pragma once
#include "pch.h"
#include "WinToolBase.h"

class WinToolSubSlider;
class WinToolSubColor;
class WinToolSub :public WinToolBase
{
	friend class WinToolSubSlider;
	friend class WinToolSubColor;
public:
	WinToolSub(WinPin* parent);
	~WinToolSub();
	D2D1_COLOR_F getSelectedColor();
	void initBtn();
	void resetVal();
	void setBorderPath();
public:
	int selectIndex2{ -1 };
	std::unique_ptr<WinToolSubSlider> slider;
	std::unique_ptr<WinToolSubColor> colorer;
protected:
	void onPaint() override;
private:
	void onCreated() override;
	BOOL onCursor() override;
	void onMouseMove(const int& x, const int& y) override;
	void onMouseLeave() override;
	void onMouseDown(const int& x, const int& y, bool isRight) override;
	void onMouseDrag(const int& x, const int& y, const UINT_PTR& modifiers) override;
	void onMouseWheel(const int& x, const int& y, const short& delta) override;
	void onDpiChanged() override;
	void paintColorSelector();
	void paintToolButtons();
	bool hoverBtn(const int& x);
private:
	float arrowX{ 0.f }, btnStart{ 0.f }, btnEnd{ 0.f };
	ComPtr<ID2D1PathGeometry> borderPath;
};

