#pragma once
#include <windows.h>

#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <wchar.h>
#include <math.h>

#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>
class MainWin
{
public:
	MainWin(HINSTANCE hinstance);
	~MainWin();
	void Paint();
private:
	void shotScreen();
	void initScaleFactor();
	void createWindow();
	void createDeviceRes();
	template<class Interface>
	inline void SafeRelease(Interface** ppInterfaceToRelease);

	static LRESULT CALLBACK WndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);


	int x, y, w, h;
	float scaleFactor;
	char* bgPixels;
	ID2D1Factory* direct2dFactory;
	ID2D1HwndRenderTarget* renderTarget;
	ID2D1SolidColorBrush* brush;
	HWND hwnd;
	HINSTANCE hinstance;
};

