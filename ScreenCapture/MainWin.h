#pragma once
#include "State.h"
#include <windows.h>
#include <windowsx.h>

#include <algorithm>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <wchar.h>
#include <math.h>
#include <iostream>
#include <vector>

#include <d2d1.h>
#include <blend2d.h>

class MainWin
{
public:
	MainWin(HINSTANCE hinstance);
	~MainWin();
	static LRESULT CALLBACK RouteWindowMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
private:
	void shotScreen();
	void initScaleFactor();
	void createWindow();
	void showWindow();
	float x, y, w, h,scaleFactor;
	HWND hwnd;
	HINSTANCE hinstance;
	State state = State::Start;
	RECT cutRect{-80,-80,-80,-80};	
	void leftBtnDown(const POINT& pos);
	void rightBtnDown(const POINT& pos);
	void mouseMove(const POINT& pos);
	void leftBtnUp(const POINT& pos);
	inline POINT getMousePoint(const LPARAM& lParam);
	POINT mouseDownPos;
	bool isLeftBtnDown;
	
	
	void initCanvas(char* bgPixels,char* boardPixels);
	void paintBoard();
	void drawMask(const POINT& pos);
	void drawPen(const POINT& pos);
	void drawEraser(const POINT& pos);
	BLImage* canvasImage;
	BLImage* bgImage; 
	BLImage* boardImage;
	BLContext* paintCtx;
	BLBox maskBoxes[8];
	BLBox cutBox;
	HBITMAP bgHbitmap;
};

