#pragma once
#include "pch.h"

class CutMask;
class WinTool;
class WinPix;
class WinCap
{
public:
	WinCap();
	~WinCap();
	static void init();
	static WinCap* get();
	void refresh();
	ComPtr<ID2D1Bitmap1> getImgData(const int& x, const int& y, const int& r, const int& b, bool hasShape = false);
public:
	int x, y, w, h;
	float dpi;
	HWND hwnd;
	ComPtr<ID2D1HwndRenderTarget> render;
	std::wstring drawState;
	std::unique_ptr<CutMask> cutMask;
private:
	void initPosSize();
	void createWindow();
	void captureScreen();
	static LRESULT CALLBACK winMsg(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void onPaint();
	void paintShape(ID2D1RenderTarget* render);
	void onMouseMove(const int& x, const int& y);
	void onMouseDrag(const int& x, const int& y);
	void onMouseDown(const int& x, const int& y, bool isRight);
	void onMouseUp(const int& x, const int& y);
	void onMouseLeave();
private:
	std::unique_ptr<WinPix> winPix;
	std::unique_ptr<WinTool> winTool;
	bool isMouseDown{ false }, mouseIn{ false };
	ComPtr<ID2D1SolidColorBrush> shapeBrush;
	ComPtr<ID2D1Bitmap1> screenImg;
};

