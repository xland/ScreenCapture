#pragma once
#include "pch.h"
#include "Win/WinBase.h"
class ShapeTextWin : public WinBase
{
public:
	ShapeTextWin(const int& x, const int& y, const int& w, const int& h);
	~ShapeTextWin();
protected:
	void onMouseDrag(const int& x, const int& y, const UINT_PTR& modifiers) override;
	//void onMouseDown(const int& x, const int& y, bool isRight);
	//void onMouseUp(const int& x, const int& y);
	//void onMouseLeave();
	//void onTimer(const int& id);
	//void onChar(const UINT& code);
	//void onKey(const UINT& key);
	//void onIME();
private:
};

