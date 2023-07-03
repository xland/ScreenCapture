#pragma once
#include "State.h"
#include <windows.h>
#include <windowsx.h>
#include <wrl/client.h>

#include <algorithm>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <wchar.h>
#include <math.h>
#include <iostream>
#include <vector>

#include <d2d1_1.h>
#include <d3d11.h>
#include <d2d1helper.h>
#include <d2d1effects.h>
#include <dwrite.h>
#include <wincodec.h>
#include <dxgi1_2.h>
#include <d2d1effects.h>
using namespace Microsoft::WRL;

class MainWin
{
public:
	MainWin(HINSTANCE hinstance);
	~MainWin();
private:


	void shotScreen();
	void initScaleFactor();
	void createWindow();
	void showWindow();
	static LRESULT CALLBACK WndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);
	float x, y, w, h,scaleFactor;
	HWND hwnd;
	HINSTANCE hinstance;

	void paint();
	void paintBg();
	void paintCanvas();
	void paintEraser();
	void paintMask();
	void initCanvas();
	State state = State::Start;
	ComPtr<ID2D1Factory1> d2dfactory1;
	ComPtr<ID2D1DeviceContext> context;
	ComPtr<IDXGISwapChain1> dxgiSwapChain;
	D2D1_RECT_F cutRect{-80.0f,-80.0f,-80.0f,-80.0f};
	ComPtr<ID2D1Bitmap1> d2dBitmap;
	ComPtr<ID2D1Bitmap1> canvasImg;
	ComPtr<ID2D1Bitmap1> eraserImg;
	
	char* bgPixels;


	void leftBtnDown(const POINT& pos);
	void rightBtnDown(const POINT& pos);
	void mouseMove(const POINT& pos);
	void leftBtnUp(const POINT& pos);
	inline POINT getMousePoint(const LPARAM& lParam);
	POINT mouseDownPos;
	bool isLeftBtnDown;


};

