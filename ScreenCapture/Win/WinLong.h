#pragma once
#include "pch.h"
#include "WinBase.h"

class CutMask;
class WinLong:public WinBase
{
public:
	WinLong(const int& x, const int& y, const int& w, const int& h);
	~WinLong();
	static void init();
	static WinLong* get();
	static void release();
	ComPtr<ID2D1Bitmap1> getCutImg();
	ComPtr<ID2D1Bitmap1> getImgByRect(D2D1_RECT_U& rect);
public:
	std::unique_ptr<CutMask> cutMask;
private:
	void onPaint() override;
	void onMouseMove(const int& x, const int& y) override;
	void onMouseDrag(const int& x, const int& y, const UINT_PTR& modifiers) override;
	void onMouseDown(const int& x, const int& y, bool isRight) override;
	void onMouseUp(const int& x, const int& y) override;
private:
	ComPtr<ID2D1Bitmap1> screenImg;
};

