#pragma once
#include "pch.h"
#include "Win/WinBase.h"
class ShapeText;
class ShapeTextWin : public WinBase
{
public:
	ShapeTextWin(const int& x, const int& y, const int& w, const int& h);
	~ShapeTextWin();
	static ShapeTextWin* get();
	void setShape(ShapeText* shape);
protected:
	void onMouseDrag(const int& x, const int& y, const UINT_PTR& modifiers) override;
	bool onCursor() override;
	void onPaint() override;
	void onMouseDown(const int& x, const int& y, bool isRight);
	void onMouseUp(const int& x, const int& y);
	//void onMouseUp(const int& x, const int& y);
	//void onMouseLeave();
	//void onIME();
private:
	void onIME() override;
	void onChar(const UINT& code);
	void onTimer(const UINT& timerId) override;
	void onKeyDown(const TCHAR& key) override;
	void setCaretByMousePos(const float& x, const float& y);
	void resetCaretPos(const int& textIndex);
	void paintSelectionBg();
	void paintSelectionBgOneRown(const int& start, const int& end);
	bool delSelection();
private:
	ShapeText* shape{ nullptr };
	bool caretVisible{ false };
	D2D1_POINT_2F caretPos;
	float caretHeight{ 0.f },padding{ 0.f };
	ComPtr<ID2D1SolidColorBrush> textSelectionBgBrush;
	int caretSelectionStart{ 0 }, caretSelectionEnd{ 0 }, caretSelectionDown{ 0 };
};

