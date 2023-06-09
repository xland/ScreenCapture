#include "MainWin.h"

namespace {
	LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (msg == WM_CREATE) {
			win = (MainWin*)(((LPCREATESTRUCT)lParam)->lpCreateParams);
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)win);
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}
		else {
			win = (MainWin*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
			if (!win) {
				return DefWindowProc(hwnd, msg, wParam, lParam);
			}
		}
		return win->winProc(hwnd, msg, wParam, lParam);
	}
}

void MainWin::createNativeWindow()
{
	std::wstring windowClassName = L"ScreenCapture_Window_Class";
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.hInstance = hinstance;
	wcex.lpfnWndProc = WindowProc;
	wcex.lpszClassName = windowClassName.c_str();
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.lpszMenuName = nullptr;
	wcex.hIconSm = LoadIcon(hinstance, (LPCTSTR)IDI_WINLOGO);
	wcex.hIcon = LoadIcon(hinstance, (LPCTSTR)IDI_WINLOGO);
	if (!RegisterClassEx(&wcex)) {
		//todo log
		MessageBox(nullptr, L"RegisterClassEx Error", L"info", MB_OK);
		return;
	}
	auto borderlessStyle = WS_EX_TOOLWINDOW | WS_POPUP | WS_SYSMENU;
	hwnd = CreateWindowEx(0, windowClassName.c_str(), L"ScreenCapture", borderlessStyle, x, y, width, height, nullptr, nullptr, hinstance, nullptr);
	if (!hwnd)
	{
		//todo log
		MessageBox(nullptr, L"CreateWindowEx Error", L"info", MB_OK);
		return;
	}
	SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)this);
	MessageBox(nullptr, L"createNativeWindow ok", L"info", MB_OK);
}

LRESULT CALLBACK MainWin::winProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	PAINTSTRUCT ps;
	switch (msg) {
		case WM_NCCALCSIZE: {
			return 0;
		}
		case WM_ACTIVATE: {
			isActive = true;
			return 0;
		}
		case WM_CLOSE: {
			::DestroyWindow(hwnd);
			return 0;
		}
		case WM_DESTROY: {
			PostQuitMessage(0);
			return 0;
		}
		case WM_PAINT:
		{
			BeginPaint(hwnd, &ps);
			paint();
			EndPaint(hwnd, &ps);
			return 0;
		}
	}
	return ::DefWindowProcW(hwnd, msg, wparam, lparam);
}