#include "MainWin.h"
#include "dwmapi.h"

LRESULT CALLBACK MainWin::RouteWindowMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_NCCREATE)
    {
        CREATESTRUCT* pCS = reinterpret_cast<CREATESTRUCT*>(lParam);
        LPVOID pThis = pCS->lpCreateParams;
        SetWindowLongPtr(hWnd, 0, reinterpret_cast<LONG_PTR>(pThis));
    }
    auto obj = reinterpret_cast<MainWin*>(GetWindowLongPtrW(hWnd, 0));
    return obj->WindowProc(hWnd, msg, wParam, lParam);
}

void MainWin::shotScreen()
{
    x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    h = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    HDC hScreen = GetDC(NULL);
    HDC hDC = CreateCompatibleDC(hScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, w, h);
    DeleteObject(SelectObject(hDC, hBitmap));
    BOOL bRet = BitBlt(hDC, 0, 0, w, h, hScreen, x, y, SRCCOPY);
    unsigned int dataSize = w * h * 4;
    auto bgPixels = new char[dataSize];
    BITMAPINFO info = { sizeof(BITMAPINFOHEADER), w, 0 - h, 1, 32, BI_RGB, dataSize, 0, 0, 0, 0 };    
    GetDIBits(hDC, hBitmap, 0, h, (LPVOID)bgPixels, &info, DIB_RGB_COLORS);
    DeleteDC(hDC);
    DeleteObject(hBitmap);

    auto boardPixels = new char[dataSize];
    bgHbitmap = CreateDIBSection(hScreen, &info, DIB_RGB_COLORS, reinterpret_cast<void**>(&boardPixels), NULL, NULL);    
    ReleaseDC(NULL, hScreen);
    initCanvas(bgPixels,boardPixels);
}

void MainWin::createWindow()
{

    WNDCLASSEX wcx{};
    wcx.cbSize = sizeof(wcx);
    wcx.style = CS_HREDRAW | CS_VREDRAW;
    wcx.lpfnWndProc = &MainWin::RouteWindowMessage;
    wcx.cbWndExtra = sizeof(MainWin*);
    wcx.hInstance = hinstance;
    wcx.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcx.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcx.lpszClassName = L"ScreenCapture";
    if (!RegisterClassEx(&wcx))
    {
        MessageBox(NULL, L"RegisterClassEx failed!", L"系统提示", NULL);
        return;
    }
    hwnd = CreateWindowEx(0, wcx.lpszClassName, wcx.lpszClassName, WS_OVERLAPPEDWINDOW, x, y, w, h, NULL, NULL, hinstance, static_cast<LPVOID>(this));
    BOOL attrib = TRUE;
    DwmSetWindowAttribute(hwnd, DWMWA_TRANSITIONS_FORCEDISABLED, &attrib, sizeof(attrib));
}

void MainWin::showWindow()
{
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
    SetCursor(LoadCursor(NULL, IDC_ARROW));
}

void MainWin::initScaleFactor()
{
    int dpi = GetDpiForWindow(hwnd);
    switch (dpi) {
    case 96:
        scaleFactor = 1.0;
        break;
    case 120:
        scaleFactor = 1.25;
        break;
    case 144:
        scaleFactor = 1.5;
        break;
    case 168:
        scaleFactor = 1.75;
        break;
    case 192:
        scaleFactor = 2.0;
        break;
    case 216:
        scaleFactor = 2.25;
        break;
    case 240:
        scaleFactor = 2.5;
        break;
    case 288:
        scaleFactor = 3.0;
        break;
    case 336:
        scaleFactor = 3.5;
        break;
    default:
        scaleFactor = 1.0;
        break;
    }
}

LRESULT CALLBACK MainWin::WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_NCCALCSIZE:
        {
            return 0;
        }
        case WM_PAINT:
        {
            paintBoard();
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            HDC hdcBmp = CreateCompatibleDC(hdc);
            DeleteObject(SelectObject(hdcBmp, bgHbitmap));
            BitBlt(hdc, 0, 0, w, h, hdcBmp, 0, 0, SRCCOPY);
            DeleteDC(hdcBmp);
            EndPaint(hwnd, &ps);
            ValidateRect(hwnd, NULL);
            return 0;
        }

        case WM_RBUTTONDOWN:
        {
            POINT point = getMousePoint(lParam);
            rightBtnDown(point);
            return 0;
        }
        case WM_MOUSEMOVE:
        {
            POINT point = getMousePoint(lParam);
            mouseMove(point);
            return 0;
        }
        case WM_LBUTTONDOWN:
        {
            POINT point = getMousePoint(lParam);
            leftBtnDown(point);
            return 0;
        }
        case WM_LBUTTONUP:
        {
            POINT point = getMousePoint(lParam);
            leftBtnUp(point);
            return 0;
        }
        case WM_ERASEBKGND:
        {
            return 1;
        }
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 1;
        }
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}


POINT MainWin::getMousePoint(const LPARAM& lParam)
{
    POINT point;
    point.x = GET_X_LPARAM(lParam);
    point.y = GET_Y_LPARAM(lParam);
    return point;
}