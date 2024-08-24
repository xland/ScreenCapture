#include "WinMax.h"
#include <memory>
#include <Windows.h>


namespace {
	std::shared_ptr<WinMax> winMax;
}

WinMax::WinMax()
{
    initSize();
    initWindow();
}

WinMax::~WinMax()
{
}

void WinMax::initSize()
{
    x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    h = GetSystemMetrics(SM_CYVIRTUALSCREEN);
}

void WinMax::Init()
{
	auto ptr = new WinMax();
	winMax = std::shared_ptr<WinMax>(ptr);
}

LRESULT WinMax::wndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    return DefWindowProc(hwnd, msg, wparam, lparam);
}
