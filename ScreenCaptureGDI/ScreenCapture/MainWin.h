#pragma once
#include <Windows.h>
#include <memory>
#include "SimpleTypes.h"
class MainWin
{
public:
	MainWin(HINSTANCE hinstance);
	~MainWin();
	LRESULT CALLBACK winProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
public:
private:
	void createNativeWindow();
	void initWindowGeomtry();
	void initWindow();
	void shotScreen();
	void render();
	void createMaskByMouse(const Point& pos);
private:
	Rect maskRect[8];
	Rect cutRect;
	Rect screenRect;
	bool isMouseDown = false;
	Point mouseDownPos;
	State state = State::Start;
	bool isActive = false;
	HINSTANCE hinstance;
	HWND hwnd;
};

