#include "WindowPin.h"

WindowPin::~WindowPin()
{
}

void WindowPin::init()
{
}

WindowPin* WindowPin::get()
{
	return nullptr;
}

LRESULT WindowPin::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	return LRESULT();
}

void WindowPin::shotScreen()
{
}

void WindowPin::initSize()
{
}

bool WindowPin::onMouseDown(const int& x, const int& y)
{
	return false;
}

bool WindowPin::onMouseDownRight(const int& x, const int& y)
{
	return false;
}

bool WindowPin::onMouseUp(const int& x, const int& y)
{
	return false;
}

bool WindowPin::onMouseMove(const int& x, const int& y)
{
	return false;
}

bool WindowPin::onMouseDrag(const int& x, const int& y)
{
	return false;
}

bool WindowPin::onChar(const unsigned int& val)
{
	return false;
}

bool WindowPin::onKeyDown(const unsigned int& val)
{
	return false;
}

bool WindowPin::onMouseWheel(const int& delta)
{
	return false;
}
