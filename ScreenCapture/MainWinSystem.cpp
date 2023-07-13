#include "MainWin.h"

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
    bgPixels = new char[dataSize];
    BITMAPINFO info = { sizeof(BITMAPINFOHEADER), w, 0 - h, 1, 32, BI_RGB, dataSize, 0, 0, 0, 0 };    
    int r = GetDIBits(hDC, hBitmap, 0, h, (LPVOID)bgPixels, &info, DIB_RGB_COLORS);
    DeleteDC(hDC);
    ReleaseDC(NULL, hScreen);
    DeleteObject(hBitmap);
}

void MainWin::createWindow()
{
    WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = MainWin::WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = sizeof(LONG_PTR);
    wcex.hInstance = hinstance;
    wcex.hbrBackground = NULL;
    wcex.lpszMenuName = NULL;
    wcex.hCursor = LoadCursor(NULL, IDI_APPLICATION);
    wcex.lpszClassName = L"ScreenCapture";
    RegisterClassEx(&wcex);
    hwnd = CreateWindow(wcex.lpszClassName, wcex.lpszClassName, WS_POPUP, x, y, w, h, NULL, NULL, hinstance, this);    
}

void MainWin::showWindow()
{
    ShowWindow(hwnd, SW_SHOWNORMAL);
    UpdateWindow(hwnd);
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

LRESULT CALLBACK MainWin::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == WM_CREATE)
    {
        LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
        MainWin* mainWin = (MainWin*)pcs->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(mainWin));
        return 1;
    }
    else
    {
        MainWin* mainWin = reinterpret_cast<MainWin*>(static_cast<LONG_PTR>(GetWindowLongPtr(hwnd, GWLP_USERDATA)));
        if (!mainWin) {
            return DefWindowProc(hwnd, message, wParam, lParam);
        }
        switch (message)
        {
            case WM_NCCALCSIZE:
            {
                return 0;
            }
            case WM_PAINT:
            {
                ValidateRect(hwnd, NULL);
                return 0;
            }
            case WM_RBUTTONDOWN:
            {
                POINT point = mainWin->getMousePoint(lParam);
                mainWin->rightBtnDown(point);
                return 0;
            }
            case WM_MOUSEMOVE:
            {
                POINT point = mainWin->getMousePoint(lParam);
                mainWin->mouseMove(point);
                return 0;
            }
            case WM_LBUTTONDOWN:
            {
                POINT point = mainWin->getMousePoint(lParam);
                mainWin->leftBtnDown(point);
                return 0;
            }
            case WM_LBUTTONUP:
            {
                POINT point = mainWin->getMousePoint(lParam);
                mainWin->leftBtnUp(point);
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
            default: {
                return DefWindowProc(hwnd, message, wParam, lParam);
            }
        }
    }
    return 0;
}
POINT MainWin::getMousePoint(const LPARAM& lParam)
{
    POINT point;
    point.x = GET_X_LPARAM(lParam);
    point.y = GET_Y_LPARAM(lParam);
    return point;
}