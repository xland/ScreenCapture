#include "pch.h"
#include "Util.h"


void Util::moveMouse(const int& key)
{
    POINT pt;
    GetCursorPos(&pt);
    switch (key) {
    case VK_LEFT: {
        SetCursorPos(pt.x - 1, pt.y);
        break;
    }
    case VK_RIGHT: {
        SetCursorPos(pt.x + 1, pt.y);
        break;
    }
    case VK_UP: {
        SetCursorPos(pt.x, pt.y - 1);
        break;
    }
    case VK_DOWN: {
        SetCursorPos(pt.x, pt.y + 1);
        break;
    }
    }
}

void Util::trackMouse(HWND hwnd, bool cancel)
{
    TRACKMOUSEEVENT tme{ sizeof(TRACKMOUSEEVENT) };
    tme.dwFlags = cancel ? TME_CANCEL | TME_LEAVE : TME_LEAVE;
    tme.hwndTrack = hwnd;
    TrackMouseEvent(&tme);
}

void Util::saveToClipboard(int& w, int& h, BYTE* data)
{
    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = -h;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    HDC screenDC = GetDC(nullptr);
    HDC memDC = CreateCompatibleDC(screenDC);
    HBITMAP hBmp = CreateCompatibleBitmap(screenDC, w, h);
    auto oldObj = SelectObject(memDC, hBmp);
    SetDIBitsToDevice(memDC, 0, 0, w, h, 0, 0, 0, h, data, &bmi, DIB_RGB_COLORS);

    OpenClipboard(nullptr);
    EmptyClipboard();
    SetClipboardData(CF_BITMAP, hBmp);
    CloseClipboard();
    SelectObject(memDC, oldObj);
    DeleteDC(memDC);
    ReleaseDC(nullptr, screenDC);
    DeleteObject(hBmp);
}

void Util::setTextToClipboard(const std::wstring& text)
{
    DataPackage dataPackage;
    dataPackage.SetText(text);
    Clipboard::SetContent(dataPackage);
    Clipboard::Flush();
}

std::wstring Util::getSaveFilePath(HWND hwnd)
{
    std::wstring result;
    IFileSaveDialog* saveDialog = nullptr;
    auto hr = CoCreateInstance(CLSID_FileSaveDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&saveDialog));
    if (FAILED(hr)) return result;

    SYSTEMTIME st;
    GetLocalTime(&st);
    std::wstring fileName = std::format(L"{:04d}{:02d}{:02d}{:02d}{:02d}{:02d}{:03d}.png",
        st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);


    DWORD dwFlags;
    saveDialog->GetOptions(&dwFlags);
    saveDialog->SetOptions(dwFlags | FOS_OVERWRITEPROMPT | FOS_STRICTFILETYPES);
    COMDLG_FILTERSPEC rgFilterSpec[] = { { L"PNG 图像", L"*.png" } };
    saveDialog->SetFileTypes(_countof(rgFilterSpec), rgFilterSpec);
    saveDialog->SetFileTypeIndex(1);
    saveDialog->SetDefaultExtension(L"png");
    saveDialog->SetFileName(fileName.data());
    hr = saveDialog->Show(hwnd);
    if (FAILED(hr)) {
        saveDialog->Release();
        return result;
    }
    IShellItem* pItem;
    hr = saveDialog->GetResult(&pItem);
    if (FAILED(hr)) {
        saveDialog->Release();
        return result;
    }
    PWSTR pszFilePath = nullptr;
    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
    if (FAILED(hr)) {
        pItem->Release();
        saveDialog->Release();
        return result;
    }
    result = pszFilePath;
    CoTaskMemFree(pszFilePath);
    return result;
}

std::wstring Util::getTextFromClipboard()
{
    if (!OpenClipboard(nullptr))
        return L"";

    HANDLE hData = GetClipboardData(CF_UNICODETEXT);
    if (!hData)
    {
        CloseClipboard();
        return L"";
    }

    wchar_t* pszText = static_cast<wchar_t*>(GlobalLock(hData));
    std::wstring text = pszText ? pszText : L"";
    GlobalUnlock(hData);
    CloseClipboard();
    return text;
}

std::vector<BYTE> Util::captureScreen(const int& x, const int& y, const int& w, const int& h)
{
    HDC hScreen = GetDC(NULL);
    HDC hDC = CreateCompatibleDC(hScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, w, h);
    auto oldObj = SelectObject(hDC, hBitmap);
    BOOL bRet = BitBlt(hDC, 0, 0, w, h, hScreen, x, y, SRCCOPY);
    ReleaseDC(NULL, hScreen);
    std::vector<BYTE> data(w * 4 * h);
    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = -h;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    GetDIBits(hDC, hBitmap, 0, h, data.data(), &bmi, DIB_RGB_COLORS);
    SelectObject(hDC, oldObj);
    DeleteDC(hDC);
    DeleteObject(hBitmap);
    return data;
}

bool Util::isInRect(const D2D1_RECT_F& rect, const float& x, const float& y)
{
    return (x > rect.left && x<rect.right && y>rect.top && y < rect.bottom);
}
