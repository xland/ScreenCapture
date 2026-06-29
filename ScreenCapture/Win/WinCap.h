#pragma once
#include "pch.h"
#include "WinBase.h"

class WinCutMask;
class WinPix;
class WinCap:public WinBase
{
public:
	WinCap(const int& x, const int& y, const int& w, const int& h);
	~WinCap();
	static void init();
	static WinCap* get();
	static void release(bool allowAutoQuit = true);
	ComPtr<ID2D1Bitmap1> getCutImg();
	ComPtr<ID2D1Bitmap1> getImgByRect(D2D1_RECT_U& rect);
public:
	std::unique_ptr<WinCutMask> cutMask;
private:
	void onCreated() override;
	void onPaint() override;
	void onMouseMove(const int& x, const int& y) override;
	void onMouseDrag(const int& x, const int& y, const UINT_PTR& modifiers) override;
	void onMouseDown(const int& x, const int& y, bool isRight) override;
	void onMouseUp(const int& x, const int& y) override;
	void onKeyDown(const UINT& key) override;
private:
	std::unique_ptr<WinPix> winPix;
	ComPtr<ID2D1Bitmap1> screenImg;
	int pressX{ 0 };
	int pressY{ 0 };
	bool isDraggingRect{ false };
};

