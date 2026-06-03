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
	void finishEdit();
public:
	bool isEditing{ false };
	D2D1_COLOR_F color;
	float pressX, pressY, fontSize,x,y;
	bool isBold{ false }, isItalic{ false };
	std::wstring text{ LR"(‌‌)" };
	ComPtr<IDWriteTextLayout> textLayout;
	ComPtr<ID2D1SolidColorBrush> textBrush;
	D2D1_RECT_F rect;
private:
};

