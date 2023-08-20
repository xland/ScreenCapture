#pragma once
#include <Windows.h>
#include <d2d1.h>
#include <blend2d.h>
class WindowBase
{
public:
	WindowBase();
	~WindowBase();
	void Show();
	void Close();
	HWND hwnd;
protected:
	void InitWindow(const int& x,const int& y,const unsigned int& w,const unsigned int& h,const bool& shadow);
private:
	static LRESULT CALLBACK RouteWindowMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	void d2DCreateRes();
	ID2D1Factory* factory;
	ID2D1HwndRenderTarget* render;
	ID2D1Bitmap* d2DImage;
	BLImage* bgImage;
	int x, y;
	unsigned int w;
	unsigned int h;
};

