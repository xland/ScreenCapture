#pragma once
#include "pch.h"
#include "ShapeBase.h"
class ShapeText : public ShapeBase
{
public:
	ShapeText(WinPin* win);
	~ShapeText();
	void paint() override;
	void paintDragger() override;
	void mouseDrag(const float& x, const float& y,const UINT_PTR& modifiers) override;
	void mouseDown(const float& x, const float& y) override;
	void mouseUp(const float& x, const float& y) override;
	void mouseMove(const float& x, const float& y) override;
	void setCursor() override;
	void setTextLayout();
public:
	D2D1_COLOR_F color;
	float pressX, pressY, fontSize;
	bool isBold{ false }, isItalic{ false };
	std::wstring text{ LR"(领导干部要胸怀两个大局，
一个是中华民族伟大复兴的战略全局，
一个是世界百年未有之大变局，
这是我们谋划工作的基本出发点。
统筹两个大局，
是贯穿习近平新时代中国特色社会主义思想的立场观点方法的重要体现，
是我们干事创业的重要思想方法和工作方法。‌‌)" };
	ComPtr<IDWriteTextLayout> textLayout;
	ComPtr<ID2D1SolidColorBrush> textBrush;
	ComPtr<IDWriteTextFormat> textFormat;
	D2D1_RECT_F rect;
private:
};

