#pragma once
#include <windows.h>
#include <windowsx.h>

#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <wchar.h>
#include <math.h>

#include <d2d1.h>
#include <d2d1helper.h>
#include <d2d1effects.h>
#include <d2d1_1.h>
#include <dwrite.h>
#include <wincodec.h>
class MainWin
{
public:
	MainWin(HINSTANCE hinstance);
	~MainWin();
	void Paint();
private:
	template<class Interface>
	inline void SafeRelease(Interface** ppInterfaceToRelease);
	HWND hwnd;
	HINSTANCE hinstance;

	void shotScreen();
	void initScaleFactor();
	void createWindow();
	static LRESULT CALLBACK WndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);
	int x, y, w, h;
	float scaleFactor;

	void createDeviceRes();
	char* bgPixels;
	ID2D1Factory* direct2dFactory;
	ID2D1HwndRenderTarget* render;
	ID2D1Bitmap* bgImg;
	ID2D1SolidColorBrush* brush;
	ID2D1SolidColorBrush* maskBrush;


	void leftBtnDown(const POINT& pos);
	void rightBtnDown(const POINT& pos);
	void mouseMove(const POINT& pos);
	void leftBtnUp(const POINT& pos);
	POINT mouseDownPos;
	bool isLeftBtnDown;

	D2D1_RECT_F masks[8];
	D2D1_RECT_F cutRect;
};

