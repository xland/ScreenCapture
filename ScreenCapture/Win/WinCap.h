#pragma once
#include "pch.h"
#include "WinBase.h"

class CutMask;
class WinPix;
class WinCap:public WinBase
{
public:
	WinCap(const int& x, const int& y, const int& w, const int& h);
	~WinCap();
	static void init();
	static WinCap* get();
	void release();
	ComPtr<ID2D1Bitmap1> getCutImg();
	ComPtr<ID2D1Bitmap1> getImgByRect(D2D1_RECT_U& rect);
public:
	std::unique_ptr<CutMask> cutMask;
private:
	void onPaint() override;
	void onMouseMove(const int& x, const int& y) override;
	void onMouseDrag(const int& x, const int& y) override;
	void onMouseDown(const int& x, const int& y, bool isRight) override;
	void onMouseUp(const int& x, const int& y) override;
	void initImg(const std::vector<BYTE>& data);
private:
	std::unique_ptr<WinPix> winPix;
	ComPtr<ID2D1Bitmap1> screenImg;
};

