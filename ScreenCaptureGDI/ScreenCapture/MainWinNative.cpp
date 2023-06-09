#include "MainWin.h"
#include <string>
//#include <gdiplusgraphics.h>

namespace {
	MainWin* win;
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
	win = this;
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
	hwnd = CreateWindowEx(0, windowClassName.c_str(), L"ScreenCapture", borderlessStyle, screenRect.x, screenRect.y, screenRect.w, screenRect.h, nullptr, nullptr, hinstance, nullptr);
	if (!hwnd)
	{
		//todo log
		MessageBox(nullptr, L"CreateWindowEx Error", L"info", MB_OK);
		return;
	}
	SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)this);
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
			//BeginPaint(hwnd, &ps);
			//SelectObject(ps.hdc, GetStockObject(GRAY_BRUSH));
			//Graphics graphics(ps.hdc);
			//// Create a Pen object.
			//Pen blackPen(Color(255, 0, 0, 0), 3);
			//// Create an array of Point objects that define the lines to draw.
			//Point point1(10, 10);
			//Point point2(10, 100);
			//Point point3(200, 50);
			//Point point4(250, 300);
			//Point points[4] = { point1, point2, point3, point4 };
			//Point* pPoints = points;
			//// Draw the lines.
			//graphics.DrawLines(&blackPen, pPoints, 4);
			//EndPaint(hwnd, &ps);
			return 0;
		}
	}
	return ::DefWindowProcW(hwnd, msg, wparam, lparam);
}