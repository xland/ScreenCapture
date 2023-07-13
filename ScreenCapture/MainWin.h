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
	State state = State::Start;
	RECT cutRect{-80.0f,-80.0f,-80.0f,-80.0f};	
	char* bgPixels;
	void leftBtnDown(const POINT& pos);
	void rightBtnDown(const POINT& pos);
	void mouseMove(const POINT& pos);
	void leftBtnUp(const POINT& pos);
	inline POINT getMousePoint(const LPARAM& lParam);
	POINT mouseDownPos;
	bool isLeftBtnDown;


};

