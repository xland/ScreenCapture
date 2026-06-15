#pragma once
#include "pch.h"
#include "WinBase.h"

class WinCutMask;
class ToolLong;
class WinLong:public WinBase
{
public:
	WinLong(const int& x, const int& y, const int& w, const int& h);
	~WinLong();
	static void init();
	static void release();
public:
	std::unique_ptr<WinCutMask> cutMask;
	void copyToClipboard();
	void saveToFile();
	void pin();
private:
	void onPaint() override;
	void onMouseMove(const int& x, const int& y) override;
	void onMouseDrag(const int& x, const int& y, const UINT_PTR& modifiers) override;
	void onMouseDown(const int& x, const int& y, bool isRight) override;
	void onMouseUp(const int& x, const int& y) override;
	void onTimer(const UINT& timerId) override;
	void onCreated() override;
	BOOL onCursor() override;
	void firstStep();
	void makeImgPreview();
	void capStep();
	void hollowWin();
	void makeTool();
	void paintImgPreview();
	void stopCap();
	void makeStopText();
private:
	bool isFinishCutMask{ false }, isShowStartBtn{ false }, isScrolling{ false }, isFinish{false};
	bool firstCheck{true};
	int dismissTime{0};
	int changeStartY{-1};
	D2D1_RECT_F stopTextRect;
	ComPtr<ID2D1SolidColorBrush> textBrush;
	ComPtr<ID2D1SolidColorBrush> bgBrush;
	ComPtr<IDWriteTextLayout> layoutTextStart;
	ComPtr<IDWriteTextLayout> layoutTextEnd;
	ComPtr<IDWriteTextFormat> textFormat;
	float startCircleR{ 30.f };
	POINT circleCenter;
	HWND targetHwnd{ nullptr };
	std::unique_ptr<ToolLong> tool;
	ComPtr<ID2D1Bitmap1> imgPreview;
	std::vector<BYTE> imgData;
	std::vector<BYTE> img1;
	int imgW{0}, imgH{0};
	int resultH{0};
	POINT capStartPos;
};

