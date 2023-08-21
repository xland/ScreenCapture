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
	void Refresh();
	int x, y;
	unsigned int w, h;
	HWND hwnd;
protected:
	virtual bool OnPaint() { return true; };
	virtual bool OnTimer(const unsigned int& id) { return true; };
	virtual bool OnRightButtonDown(const int& x, const int& y) { return false; };
	virtual bool OnRightButtonUp(const int& x, const int& y) { return false; };
	virtual bool OnLeftButtonDown(const int& x, const int& y) { return false; };
	virtual bool OnLeftButtonUp(const int& x, const int& y) { return false; };
	virtual bool OnMouseMove(const int& x, const int& y) { return false; };
	virtual bool OnKeyDown(const unsigned int& key) { return false; };
	virtual bool OnKeyUp(const unsigned int& key) { return false; };
	virtual bool OnChar(const unsigned int& key) { return true; };
	void InitWindow(const bool& shadow);
	unsigned char* pixelData;
private:
	static LRESULT CALLBACK RouteWindowMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	bool paint();
	void initScaleFactor();
	float scaleFactor;
	ID2D1Factory* factory;
	ID2D1HwndRenderTarget* render;
	ID2D1Bitmap* d2DImage;
};

