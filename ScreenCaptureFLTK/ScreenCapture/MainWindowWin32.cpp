#include "MainWindow.h"

namespace {
	LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		MainWindow* win;
		if (msg == WM_CREATE) {
			win = (MainWindow*)(((LPCREATESTRUCT)lParam)->lpCreateParams);
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)win);
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}
		else {
			win = (MainWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
			if (!win) {
				return DefWindowProc(hwnd, msg, wParam, lParam);
			}
		}
		return mainWindow->winProc(hwnd, msg, wParam, lParam);
	}
}

void MainWindow::createWin32Window()
{
	//std::wstring windowClassName = L"ScreenCapture_Window_Class";
	//WNDCLASSEX wcex;
	//wcex.cbSize = sizeof(WNDCLASSEX);
	//wcex.style = CS_HREDRAW | CS_VREDRAW;
	//wcex.hInstance = hinstance;
	//wcex.lpfnWndProc = WindowProc;
	//wcex.lpszClassName = windowClassName.c_str();
	//wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	//wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	//wcex.cbClsExtra = 0;
	//wcex.cbWndExtra = 0;
	//wcex.lpszMenuName = nullptr;
	//wcex.hIconSm = LoadIcon(hinstance, (LPCTSTR)IDI_WINLOGO);
	//wcex.hIcon = LoadIcon(hinstance, (LPCTSTR)IDI_WINLOGO);
	//if (!RegisterClassEx(&wcex)) {
	//	//todo log
	//	return;
	//}
	//x = GetSystemMetrics(SM_XVIRTUALSCREEN);
	//y = GetSystemMetrics(SM_YVIRTUALSCREEN);
	//width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	//height = GetSystemMetrics(SM_CYVIRTUALSCREEN);
	//auto borderlessStyle = WS_EX_TOOLWINDOW | WS_POPUP | WS_VISIBLE | WS_SYSMENU;
	//hwnd = CreateWindowEx(0, windowClassName.c_str(), L"ScreenCapture", borderlessStyle, x, y, width, height, nullptr, nullptr, hinstance, nullptr);
	//if (!hwnd)
	//{
	//	//todo log
	//	return;
	//}
	//SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)this);
	//return;
}

LRESULT CALLBACK  MainWindow::winProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	switch (msg) {
	case WM_NCCALCSIZE: {
		return 0;
	}
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}
	case WM_PAINT:
	{
		//BeginPaint(hwnd, &ps);
		//paint();
		//EndPaint(hwnd, &ps);
		return 0;
	}
	case WM_ERASEBKGND: {
		return 0; // InvalidateRect(win->hwnd, nullptr, false);
	}
	case WM_RBUTTONDOWN:
	{
		return 0;
	}
	case WM_GETMINMAXINFO:
	{
		//MINMAXINFO* mminfo;
		//mminfo = (PMINMAXINFO)lParam;
		//mminfo->ptMinTrackSize.x = minWidth;
		//mminfo->ptMinTrackSize.y = minHeight;
		//mminfo->ptMaxPosition.x = 0;
		//mminfo->ptMaxPosition.y = 0;
		return 0;
	}
	case WM_SIZE: {
		//clientWidth = LOWORD(lParam);
		//clientHeight = HIWORD(lParam);
		//App::get()->Resize(clientWidth, clientHeight);
		return 0;
	}
	case WM_MOUSELEAVE: {
		//auto x = GET_X_LPARAM(lParam);
		//auto y = GET_Y_LPARAM(lParam);
		//App::get()->CheckMouseEnter(x, y);
		////HideToolTip();
		//setTracking(false);
		return 0;
	}
	case WM_MOUSEWHEEL:
	{
		//auto delta = GET_WHEEL_DELTA_WPARAM(wParam); //ÉÏ120£¬ÏÂ-120
		//if (delta > 0)
		//{
		//	App::get()->imageViewer->Zoom(1.02f);
		//}
		//else
		//{
		//	App::get()->imageViewer->Zoom(0.98f);
		//}
		return 0;
	}
	case WM_MOUSEMOVE:
	{
		//auto x = GET_X_LPARAM(lParam);
		//auto y = GET_Y_LPARAM(lParam);
		//App::get()->CheckMouseEnter(x, y);
		//setTracking(true);
		return 0;
	}
	case WM_LBUTTONDOWN:
	{
		//auto x = GET_X_LPARAM(lParam);
		//auto y = GET_Y_LPARAM(lParam);
		//SetCapture(hwnd);
		//App::get()->CheckMouseDown(x, y);
		return 0;
	}
	case WM_LBUTTONUP:
	{
		//GetDoubleClickTime();
		//auto x = GET_X_LPARAM(lParam);
		//auto y = GET_Y_LPARAM(lParam);
		//ReleaseCapture();
		//App::get()->CheckMouseUp(x, y);
		//OutputDebugStringA("CheckMouseUp");
		//OutputDebugStringA("\r\n");

		//return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}