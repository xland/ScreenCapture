#include <Windows.h>
#include <windowsx.h>
#include <dwmapi.h>
#include <iostream>
#include <string>
#pragma comment(lib, "dwmapi.lib")
LRESULT hitTest(HWND hwnd, POINT cursor) {
    const POINT border{
        ::GetSystemMetrics(SM_CXFRAME) + ::GetSystemMetrics(SM_CXPADDEDBORDER),
        ::GetSystemMetrics(SM_CYFRAME) + ::GetSystemMetrics(SM_CXPADDEDBORDER)
    };
    RECT winRect;
    if (!::GetWindowRect(hwnd, &winRect)) return HTNOWHERE;
    enum region_mask {
        client = 0b0000,
        left = 0b0001,
        right = 0b0010,
        top = 0b0100,
        bottom = 0b1000,
    };
    const auto result = left * (cursor.x < (winRect.left + border.x)) |
        right * (cursor.x >= (winRect.right - border.x)) |
        top * (cursor.y < (winRect.top + border.y)) |
        bottom * (cursor.y >= (winRect.bottom - border.y));
    switch (result) {
    case left: return HTLEFT;
    case right: return HTRIGHT;
    case top: return HTTOP;
    case bottom: return HTBOTTOM;
    case top | left: return HTTOPLEFT;
    case top | right: return HTTOPRIGHT;
    case bottom | left: return HTBOTTOMLEFT;
    case bottom | right: return HTBOTTOMRIGHT;
    case client: return HTCAPTION;
    default: return HTNOWHERE;
    }
}
LRESULT CALLBACK wndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    switch (msg) {
    case WM_NCCALCSIZE: {
        return 0;
    }
    case WM_NCHITTEST: {
        return hitTest(hwnd, POINT{ GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) });
    }
    case WM_CLOSE: {
        ::DestroyWindow(hwnd);
        return 0;
    }
    case WM_DESTROY: {
        PostQuitMessage(0);
        return 0;
    }
    }
    return ::DefWindowProcW(hwnd, msg, wparam, lparam);
}

void GetScreenShot(void)
{
    int x1, y1, x2, y2, w, h;

    // get screen dimensions
    x1 = GetSystemMetrics(SM_XVIRTUALSCREEN);
    y1 = GetSystemMetrics(SM_YVIRTUALSCREEN);
    x2 = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    y2 = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    w = x2 - x1;
    h = y2 - y1;

    // copy screen to bitmap
    HDC     hScreen = GetDC(NULL);
    HDC     hDC = CreateCompatibleDC(hScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, w, h);
    HGDIOBJ old_obj = SelectObject(hDC, hBitmap);
    BOOL    bRet = BitBlt(hDC, 0, 0, w, h, hScreen, x1, y1, SRCCOPY);

    // save bitmap to clipboard
    OpenClipboard(NULL);
    EmptyClipboard();
    SetClipboardData(CF_BITMAP, hBitmap);
    CloseClipboard();

    // clean up
    SelectObject(hDC, old_obj);
    DeleteDC(hDC);
    ReleaseDC(NULL, hScreen);
    DeleteObject(hBitmap);
}

BOOL WINAPI SaveBitmap(WCHAR* wPath)
{
    BITMAPFILEHEADER bfHeader;
    BITMAPINFOHEADER biHeader;
    BITMAPINFO bInfo;
    HGDIOBJ hTempBitmap;
    HBITMAP hBitmap;
    BITMAP bAllDesktops;
    HDC hDC, hMemDC;
    LONG lWidth, lHeight;
    BYTE* bBits = NULL;
    HANDLE hHeap = GetProcessHeap();
    DWORD cbBits, dwWritten = 0;
    HANDLE hFile;
    INT x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    INT y = GetSystemMetrics(SM_YVIRTUALSCREEN);

    ZeroMemory(&bfHeader, sizeof(BITMAPFILEHEADER));
    ZeroMemory(&biHeader, sizeof(BITMAPINFOHEADER));
    ZeroMemory(&bInfo, sizeof(BITMAPINFO));
    ZeroMemory(&bAllDesktops, sizeof(BITMAP));

    hDC = GetDC(NULL);
    hTempBitmap = GetCurrentObject(hDC, OBJ_BITMAP);
    GetObjectW(hTempBitmap, sizeof(BITMAP), &bAllDesktops);

    lWidth = bAllDesktops.bmWidth;
    lHeight = bAllDesktops.bmHeight;

    DeleteObject(hTempBitmap);

    bfHeader.bfType = (WORD)('B' | ('M' << 8));
    bfHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    biHeader.biSize = sizeof(BITMAPINFOHEADER);
    biHeader.biBitCount = 24;
    biHeader.biCompression = BI_RGB;
    biHeader.biPlanes = 1;
    biHeader.biWidth = lWidth;
    biHeader.biHeight = lHeight;

    bInfo.bmiHeader = biHeader;

    cbBits = (((24 * lWidth + 31) & ~31) / 8) * lHeight;

    hMemDC = CreateCompatibleDC(hDC);
    hBitmap = CreateDIBSection(hDC, &bInfo, DIB_RGB_COLORS, (VOID**)&bBits, NULL, 0);
    SelectObject(hMemDC, hBitmap);
    BitBlt(hMemDC, 0, 0, lWidth, lHeight, hDC, x, y, SRCCOPY);


    hFile = CreateFileW(wPath, GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (INVALID_HANDLE_VALUE == hFile)
    {
        DeleteDC(hMemDC);
        ReleaseDC(NULL, hDC);
        DeleteObject(hBitmap);

        return FALSE;
    }
    WriteFile(hFile, &bfHeader, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
    WriteFile(hFile, &biHeader, sizeof(BITMAPINFOHEADER), &dwWritten, NULL);
    WriteFile(hFile, bBits, cbBits, &dwWritten, NULL);
    FlushFileBuffers(hFile);
    CloseHandle(hFile);

    DeleteDC(hMemDC);
    ReleaseDC(NULL, hDC);
    DeleteObject(hBitmap);

    return TRUE;
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    WNDCLASSEXW wcx{};
    wcx.cbSize = sizeof(wcx);
    wcx.style = CS_HREDRAW | CS_VREDRAW;
    wcx.hInstance = nullptr;
    wcx.lpfnWndProc = &wndProc;
    wcx.lpszClassName = L"BorderlessWindowClass";
    wcx.hbrBackground = CreateSolidBrush(RGB(226, 160, 160));
    wcx.hCursor = ::LoadCursorW(nullptr, IDC_ARROW);
    ::RegisterClassExW(&wcx);
    int x1 = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int y1 = GetSystemMetrics(SM_YVIRTUALSCREEN);
    unsigned int vScreenWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    unsigned int vScreenHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    auto borderlessStyle = WS_POPUP | WS_THICKFRAME | WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_VISIBLE;
    HWND hwnd = CreateWindowExW(0, wcx.lpszClassName, L"´°¿Ú±êÌâ", borderlessStyle, x1, y1, vScreenWidth, vScreenHeight, nullptr, nullptr, nullptr, nullptr);
    //GetScreenShot();

    std::wstring str = L"screenall.png";
    SaveBitmap(str.data());

    MSG msg;
    while (::GetMessageW(&msg, nullptr, 0, 0) == TRUE) {
        ::TranslateMessage(&msg);
        ::DispatchMessageW(&msg);
    }
}