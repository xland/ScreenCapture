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
    if (!OpenClipboard(nullptr)) return;
    EmptyClipboard();
    // 直接将原始像素写入全局内存，以 CF_DIB 写入剪切板
    // 避免 CreateCompatibleBitmap 引入屏幕 DPI 元数据导致粘贴时缩放模糊
    DWORD rowBytes = (DWORD)w * 4;
    DWORD imgBytes = rowBytes * (DWORD)h;
    HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, sizeof(BITMAPINFOHEADER) + imgBytes);
    if (!hGlobal) return;
    auto* p = static_cast<BYTE*>(GlobalLock(hGlobal));
    if (!p) { GlobalFree(hGlobal); return; }
    auto* bih = reinterpret_cast<BITMAPINFOHEADER*>(p);
    *bih = {};
    bih->biSize        = sizeof(BITMAPINFOHEADER);
    bih->biWidth       = w;
    bih->biHeight      = -h;      // 负数 = top-down DIB
    bih->biPlanes      = 1;
    bih->biBitCount    = 32;
    bih->biCompression = BI_RGB;
    bih->biSizeImage   = imgBytes;
    CopyMemory(p + sizeof(BITMAPINFOHEADER), data, imgBytes);
    GlobalUnlock(hGlobal);
    if (!SetClipboardData(CF_DIB, hGlobal)) {
        GlobalFree(hGlobal);
        CloseClipboard();
        return;
    }
    CloseClipboard();
    // SetClipboardData 成功后剪切板接管 hGlobal，不可再 GlobalFree
}

bool Util::saveToFile(const std::wstring& path, const int& w, const int& h, BYTE* data)
{
    if (path.empty() || w <= 0 || h <= 0 || !data) return false;
    UINT rowBytes = (UINT)w * 4;
    UINT imgBytes = rowBytes * (UINT)h;

    ComPtr<IWICImagingFactory> factory;
    auto hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(factory.GetAddressOf()));
    if (FAILED(hr)) return false;

    ComPtr<IWICStream> stream;
    hr = factory->CreateStream(stream.GetAddressOf());
    if (FAILED(hr)) return false;
    hr = stream->InitializeFromFilename(path.c_str(), GENERIC_WRITE);
    if (FAILED(hr)) return false;

    ComPtr<IWICBitmapEncoder> encoder;
    hr = factory->CreateEncoder(GUID_ContainerFormatPng, nullptr, encoder.GetAddressOf());
    if (FAILED(hr)) return false;
    hr = encoder->Initialize(stream.Get(), WICBitmapEncoderNoCache);
    if (FAILED(hr)) return false;

    ComPtr<IWICBitmapFrameEncode> frame;
    hr = encoder->CreateNewFrame(frame.GetAddressOf(), nullptr);
    if (FAILED(hr)) return false;
    hr = frame->Initialize(nullptr);
    if (FAILED(hr)) return false;
    hr = frame->SetSize((UINT)w, (UINT)h);
    if (FAILED(hr)) return false;

    WICPixelFormatGUID format = GUID_WICPixelFormat32bppBGRA;
    hr = frame->SetPixelFormat(&format);
    if (FAILED(hr) || !IsEqualGUID(format, GUID_WICPixelFormat32bppBGRA)) return false;

    hr = frame->WritePixels((UINT)h, rowBytes, imgBytes, data);
    if (FAILED(hr)) return false;
    hr = frame->Commit();
    if (FAILED(hr)) return false;
    hr = encoder->Commit();
    return SUCCEEDED(hr);
}

void Util::setTextToClipboard(const std::wstring& text)
{
    //DataPackage dataPackage;
    //dataPackage.SetText(text);
    //Clipboard::SetContent(dataPackage);
    //Clipboard::Flush();
}

std::wstring Util::getSaveFilePath(HWND hwnd, const std::wstring& ext)
{
    std::wstring result;
    ComPtr<IFileSaveDialog> saveDialog;
    auto hr = CoCreateInstance(CLSID_FileSaveDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&saveDialog));
    if (FAILED(hr)) return result;
    DWORD dwFlags;
    saveDialog->GetOptions(&dwFlags);
    saveDialog->SetOptions(dwFlags | FOS_OVERWRITEPROMPT | FOS_STRICTFILETYPES);
    auto xx = L"*." + ext;
    COMDLG_FILTERSPEC rgFilterSpec[] = { { L"文件", xx.data()}};
    saveDialog->SetFileTypes(_countof(rgFilterSpec), rgFilterSpec);
    saveDialog->SetFileTypeIndex(1);
    saveDialog->SetDefaultExtension(ext.data());
    std::wstring fileName = createFileName(ext);
    saveDialog->SetFileName(fileName.data());
    hr = saveDialog->Show(hwnd);
    if (FAILED(hr)) return result;
    ComPtr<IShellItem> pItem;
    hr = saveDialog->GetResult(&pItem);
    if (FAILED(hr)) return result;
    PWSTR pszFilePath = nullptr;
    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
    if (FAILED(hr)) return result;
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

std::tuple<int, int, int, int> Util::getDesktopInfo()
{
    return std::make_tuple(
        GetSystemMetrics(SM_XVIRTUALSCREEN),
        GetSystemMetrics(SM_YVIRTUALSCREEN),
        GetSystemMetrics(SM_CXVIRTUALSCREEN),
        GetSystemMetrics(SM_CYVIRTUALSCREEN)
    );
}

std::wstring Util::createFileName(const std::wstring& ext)
{
    SYSTEMTIME st;
    GetLocalTime(&st);
    std::wstring fileName = std::format(L"{:04d}{:02d}{:02d}{:02d}{:02d}{:02d}{:03d}.{}",
        st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds,ext);
    return fileName;
}
std::wstring Util::convertToWStr(const char* str)
{
    if (!str) return std::wstring();
    int count = MultiByteToWideChar(CP_UTF8, 0, str, -1, 0, 0);
    if (count == 0) return std::wstring();
    std::vector<wchar_t> buffer(count);
    MultiByteToWideChar(CP_UTF8, 0, str, -1, buffer.data(), count);
    return std::wstring(buffer.data(), buffer.size() - 1);
}

std::string Util::convertToStr(const std::wstring& wstr)
{
    const int count = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.length(), NULL, 0, NULL, NULL);
    std::string str(count, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], count, NULL, NULL);
    return str;
}

void Util::addFileToClipboard(const std::wstring& filePath)
{
    if (!OpenClipboard(NULL)) return;
    EmptyClipboard();
    size_t totalSize = sizeof(DROPFILES);
    totalSize += (filePath.length() + 1) * sizeof(wchar_t);
    totalSize += sizeof(wchar_t);
    HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, totalSize);
    if (!hGlobal) {
        CloseClipboard();
        return;
    }
    DROPFILES* pDropFiles = static_cast<DROPFILES*>(GlobalLock(hGlobal));
    if (!pDropFiles) {
        GlobalFree(hGlobal);
        CloseClipboard();
        return;
    }
    pDropFiles->pFiles = sizeof(DROPFILES);
    pDropFiles->fWide = TRUE;
    wchar_t* dest = reinterpret_cast<wchar_t*>(pDropFiles + 1);
    wcscpy_s(dest, filePath.length() + 1, filePath.c_str());
    dest += filePath.length() + 1;
    *dest = L'\0';
    GlobalUnlock(hGlobal);
    if (!SetClipboardData(CF_HDROP, hGlobal)) {
        GlobalFree(hGlobal);
        CloseClipboard();
    }
    CloseClipboard();
}