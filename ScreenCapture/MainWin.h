#pragma once
#include <windows.h>
#include <windowsx.h>
#include <wrl/client.h>

#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <wchar.h>
#include <math.h>

#include <d3d11.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <d2d1effects.h>
#include <d2d1_1.h>
#include <dwrite.h>
#include <wincodec.h>
#include <dxgi1_2.h>
using namespace Microsoft::WRL;

class MainWin
{
public:
	MainWin(HINSTANCE hinstance);
	~MainWin();
private:
	template <class T> void release(T** ppT)
	{
		if (*ppT)
		{
			(*ppT)->Release();
			*ppT = NULL;
		}
	}


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
	void paintMask();
	void initCanvas();
	ComPtr<ID2D1DeviceContext> context;
	ComPtr<IDXGISwapChain1> dxgiSwapChain;
	D2D1_RECT_F cutRect;


	//ID2D1Layer* maskLayer;
	char* bgPixels;
	//ID2D1Factory* factory;
	//ID2D1HwndRenderTarget* render;	
	//ID2D1SolidColorBrush* brush;
	//ID2D1SolidColorBrush* maskBrush;
	//D2D1_RECT_F cutRect;


	void leftBtnDown(const POINT& pos);
	void rightBtnDown(const POINT& pos);
	void mouseMove(const POINT& pos);
	void leftBtnUp(const POINT& pos);
	inline POINT getMousePoint(const LPARAM& lParam);
	POINT mouseDownPos;
	bool isLeftBtnDown;


};

