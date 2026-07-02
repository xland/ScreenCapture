#include "pch.h"
#include "Util.h"
#include "Lang.h"


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
    if (w <= 0 || h <= 0 || !data) return;

    // 目标：同时提供 CF_DIBV5（Office/微信/WPS 等原生应用）和 "PNG" 注册格式（浏览器/Electron 应用）
    // 输入像素为 BGRA、top-down 32bpp

    DWORD rowBytes = (DWORD)w * 4;
    DWORD imgBytes = rowBytes * (DWORD)h;

    // ---------- 1) 用 WIC 把像素编码为 PNG 到内存流 ----------
    ComPtr<IStream> pngStream;
    HRESULT hr = CreateStreamOnHGlobal(nullptr, TRUE, pngStream.GetAddressOf());
    if (FAILED(hr)) return;

    ComPtr<IWICImagingFactory> factory;
    hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(factory.GetAddressOf()));
    if (FAILED(hr)) return;

    ComPtr<IWICBitmapEncoder> encoder;
    hr = factory->CreateEncoder(GUID_ContainerFormatPng, nullptr, encoder.GetAddressOf());
    if (FAILED(hr)) return;
    hr = encoder->Initialize(pngStream.Get(), WICBitmapEncoderNoCache);
    if (FAILED(hr)) return;

    ComPtr<IWICBitmapFrameEncode> frame;
    hr = encoder->CreateNewFrame(frame.GetAddressOf(), nullptr);
    if (FAILED(hr)) return;
    hr = frame->Initialize(nullptr);
    if (FAILED(hr)) return;
    hr = frame->SetSize((UINT)w, (UINT)h);
    if (FAILED(hr)) return;
    WICPixelFormatGUID fmt = GUID_WICPixelFormat32bppBGRA;
    hr = frame->SetPixelFormat(&fmt);
    if (FAILED(hr) || !IsEqualGUID(fmt, GUID_WICPixelFormat32bppBGRA)) return;
    hr = frame->WritePixels((UINT)h, rowBytes, imgBytes, data);
    if (FAILED(hr)) return;
    hr = frame->Commit();
    if (FAILED(hr)) return;
    hr = encoder->Commit();
    if (FAILED(hr)) return;

    // 取出编码后的实际字节，拷到一块精确大小的 HGLOBAL
    STATSTG stat{};
    if (FAILED(pngStream->Stat(&stat, STATFLAG_NONAME))) return;
    SIZE_T pngSize = (SIZE_T)stat.cbSize.QuadPart;
    if (pngSize == 0) return;

    HGLOBAL hPngSrc = nullptr;
    if (FAILED(GetHGlobalFromStream(pngStream.Get(), &hPngSrc)) || !hPngSrc) return;
    void* srcPtr = GlobalLock(hPngSrc);
    if (!srcPtr) return;

    HGLOBAL hPng = GlobalAlloc(GMEM_MOVEABLE, pngSize);
    if (!hPng) { GlobalUnlock(hPngSrc); return; }
    void* dstPtr = GlobalLock(hPng);
    if (!dstPtr) { GlobalUnlock(hPngSrc); GlobalFree(hPng); return; }
    memcpy(dstPtr, srcPtr, pngSize);
    GlobalUnlock(hPng);
    GlobalUnlock(hPngSrc);

    // ---------- 2) 构造 CF_DIBV5（带 alpha） ----------
    HGLOBAL hDibV5 = GlobalAlloc(GMEM_MOVEABLE, sizeof(BITMAPV5HEADER) + imgBytes);
    if (!hDibV5) { GlobalFree(hPng); return; }
    auto* pv5 = static_cast<BYTE*>(GlobalLock(hDibV5));
    if (!pv5) { GlobalFree(hDibV5); GlobalFree(hPng); return; }
    auto* bv5 = reinterpret_cast<BITMAPV5HEADER*>(pv5);
    *bv5 = {};
    bv5->bV5Size        = sizeof(BITMAPV5HEADER);
    bv5->bV5Width       = w;
    bv5->bV5Height      = -h;                 // 负 = top-down
    bv5->bV5Planes      = 1;
    bv5->bV5BitCount    = 32;
    bv5->bV5Compression = BI_BITFIELDS;       // 让接收端识别 alpha
    bv5->bV5SizeImage   = imgBytes;
    bv5->bV5RedMask     = 0x00FF0000;
    bv5->bV5GreenMask   = 0x0000FF00;
    bv5->bV5BlueMask    = 0x000000FF;
    bv5->bV5AlphaMask   = 0xFF000000;
    bv5->bV5CSType      = LCS_sRGB;
    bv5->bV5Intent      = LCS_GM_GRAPHICS;
    CopyMemory(pv5 + sizeof(BITMAPV5HEADER), data, imgBytes);
    GlobalUnlock(hDibV5);

    // ---------- 3) 写入剪切板 ----------
    if (!OpenClipboard(nullptr)) {
        GlobalFree(hDibV5);
        GlobalFree(hPng);
        return;
    }
    EmptyClipboard();

    // CF_DIBV5：设置成功后所有权移交剪切板
    if (!SetClipboardData(CF_DIBV5, hDibV5)) {
        GlobalFree(hDibV5);
    }

    // "PNG" 注册格式：浏览器/Electron 应用会认这个
    UINT cfPng = RegisterClipboardFormatW(L"PNG");
    if (cfPng == 0 || !SetClipboardData(cfPng, hPng)) {
        GlobalFree(hPng);
    }

    CloseClipboard();
    // 成功写入的 HGLOBAL 归剪切板所有，不再 GlobalFree
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
    if (!OpenClipboard(NULL)) return;
    EmptyClipboard();
    size_t length = (text.size() + 1) * sizeof(wchar_t);
    HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, length);
    if (hGlobal == NULL) {
        CloseClipboard();
        return;
    }
    auto pGlobal = (wchar_t*)GlobalLock(hGlobal);
    if (pGlobal == NULL) {
        CloseClipboard();
        return;
    }
    memcpy(pGlobal, text.data(), length);
    GlobalUnlock(hGlobal);
    SetClipboardData(CF_UNICODETEXT, hGlobal);
    CloseClipboard();
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
    COMDLG_FILTERSPEC rgFilterSpec[] = { { Lang::get(L"util.file").c_str(), xx.data()}};
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
    if (FAILED(hr)) return result; //RPC_E_WRONG_THREAD The application called an interface that was marshalled for a different thread.
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

std::wstring Util::readFile(const std::wstring& path)
{
    HANDLE hFile = CreateFile(path.data(), GENERIC_READ,  FILE_SHARE_READ,   NULL, OPEN_EXISTING,  FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) return L"";
    LARGE_INTEGER fileSize;
    if (!GetFileSizeEx(hFile, &fileSize)) {
        CloseHandle(hFile);
        return L"";
    }
    if (fileSize.QuadPart == 0) {
        CloseHandle(hFile);
        return L"";
    }
    std::wstring outContent;
    outContent.resize(static_cast<size_t>(fileSize.QuadPart));
    DWORD bytesRead = 0;
    BOOL success = ReadFile(hFile, outContent.data(), static_cast<DWORD>(fileSize.QuadPart), &bytesRead, NULL );
    if (!success) {
        CloseHandle(hFile);
        return L"";
    }
    outContent.resize(bytesRead);
    CloseHandle(hFile);
    if (outContent.size() >= 1 && outContent[0] == 0xFEFF) {
        outContent.erase(0, 1); // 删除第一个字符
    }
    return outContent;
}

void Util::saveFile(const std::wstring& path, const std::wstring& content)
{
    HANDLE hFile = CreateFile(path.data(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) return;
    DWORD bytesWritten = 0;
    WORD bom = 0xFEFF;
    BOOL success = WriteFile(hFile, &bom, sizeof(WORD), &bytesWritten, NULL);
    if (!success) {
        CloseHandle(hFile); // 出错也要关闭句柄
        return;
    }
    DWORD contentLength = static_cast<DWORD>(content.length() * sizeof(wchar_t));
    success = WriteFile(hFile, content.data(), contentLength, &bytesWritten, NULL);
    if (!success) {
        CloseHandle(hFile);
        return;
    }
    CloseHandle(hFile);
}

std::vector<std::wstring> Util::splitStr(const std::wstring& str, wchar_t delimiter) {
    std::vector<std::wstring> result;
    std::wistringstream wiss(str); // 将字符串转为宽字符流
    std::wstring token;
    while (std::getline(wiss, token, delimiter)) {
        result.push_back(token);
    }
    return result;
}

std::wstring Util::keyToStr(UINT vkCode)
{
    switch (vkCode) {
        // --- 修饰键 ---
    case VK_CONTROL: return L"Ctrl";
    case VK_MENU:    return L"Alt";
    case VK_SHIFT:   return L"Shift";
    case VK_LWIN:    return L"LWin";
    case VK_RWIN:    return L"RWin";
    case VK_CAPITAL: return L"CapsLock";
        // --- 功能键 (F1 - F12) ---
    case VK_F1: return L"F1";
    case VK_F2: return L"F2";
    case VK_F3: return L"F3";
    case VK_F4: return L"F4";
    case VK_F5: return L"F5";
    case VK_F6: return L"F6";
    case VK_F7: return L"F7";
    case VK_F8: return L"F8";
    case VK_F9: return L"F9";
    case VK_F10: return L"F10";
    case VK_F11: return L"F11";
    case VK_F12: return L"F12";
        // --- 方向键 ---
    case VK_UP: return L"Up";
    case VK_DOWN: return L"Down";
    case VK_LEFT: return L"Left";
    case VK_RIGHT: return L"Right";
        // --- 控制与编辑键 ---
    case VK_RETURN:  return L"Enter";
    case VK_ESCAPE:  return L"Esc";
    case VK_TAB:     return L"Tab";
    case VK_SPACE:   return L"Space";
    case VK_BACK:    return L"Backspace";
    case VK_DELETE:  return L"Delete";
    case VK_INSERT:  return L"Insert";
    case VK_HOME:    return L"Home";
    case VK_END:     return L"End";
    case VK_PRIOR:   return L"PageUp";
    case VK_NEXT:    return L"PageDown";
    case VK_SNAPSHOT:return L"PrintScreen";
    case VK_SCROLL:  return L"ScrollLock";
    case VK_PAUSE:   return L"Pause";
        // --- 小键盘 ---
    case VK_NUMLOCK: return L"NumLock";
    case VK_MULTIPLY: return L"*";
    case VK_ADD:      return L"+";
    case VK_SUBTRACT: return L"-";
    case VK_DIVIDE:   return L"/";
    case VK_DECIMAL:  return L".";
    default:
        // --- 字母键 (A-Z 对应 ASCII 码 65-90) ---
        if (vkCode >= 'A' && vkCode <= 'Z') {
            return std::wstring(1, static_cast<wchar_t>(vkCode));
        }
        // --- 主键盘数字键 (0-9 对应 ASCII 码 48-57) ---
        if (vkCode >= '0' && vkCode <= '9') {
            return std::wstring(1, static_cast<wchar_t>(vkCode));
        }
        // --- 小键盘数字键 (0-9) ---
        if (vkCode >= VK_NUMPAD0 && vkCode <= VK_NUMPAD9) {
            return L"Num"+std::to_wstring(vkCode - VK_NUMPAD0);
        }
        // --- 未识别的键，返回带前缀的数字 ---
        return L"";
    }
}

UINT Util::strToKey(const std::wstring& lowerName)
{
    if (lowerName.empty()) return 0;
    // 1. 处理单字符 (A-Z, 0-9)
    if (lowerName.length() == 1) {
        wchar_t c = lowerName[0];
        if (c >= 'A' && c <= 'Z') return static_cast<UINT>(c);
        if (c >= 'a' && c <= 'z') return static_cast<UINT>(std::towupper(c));
        if (c >= '0' && c <= '9') return static_cast<UINT>(c);
    }
    // 2. 处理小键盘数字 "Num0" 等
    if (lowerName.rfind(L"num", 0) == 0) {
        std::wstring numPart = lowerName.substr(lowerName.find_first_not_of(L" \t", 3));
        if (!numPart.empty() && numPart.length() == 1) {
            wchar_t c = numPart[0];
            if (c >= '0' && c <= '9') {
                return VK_NUMPAD0 + (c - '0');
            }
        }
    }
    // 3. 功能键 (F1 - F12)
    if (lowerName == L"f1") return VK_F1;
    if (lowerName == L"f2") return VK_F2;
    if (lowerName == L"f3") return VK_F3;
    if (lowerName == L"f4") return VK_F4;
    if (lowerName == L"f5") return VK_F5;
    if (lowerName == L"f6") return VK_F6;
    if (lowerName == L"f7") return VK_F7;
    if (lowerName == L"f8") return VK_F8;
    if (lowerName == L"f9") return VK_F9;
    if (lowerName == L"f10") return VK_F10;
    if (lowerName == L"f11") return VK_F11;
    if (lowerName == L"f12") return VK_F12;
    // 4. 方向键
    if (lowerName == L"up") return VK_UP;
    if (lowerName == L"down") return VK_DOWN;
    if (lowerName == L"left") return VK_LEFT;
    if (lowerName == L"right") return VK_RIGHT;
    // 5. 控制与编辑键
    if (lowerName == L"enter") return VK_RETURN;
    if (lowerName == L"esc") return VK_ESCAPE;
    if (lowerName == L"space") return VK_SPACE;
    if (lowerName == L"tab") return VK_TAB;
    if (lowerName == L"backspace") return VK_BACK;
    if (lowerName == L"delete") return VK_DELETE;
    if (lowerName == L"insert") return VK_INSERT;
    if (lowerName == L"home") return VK_HOME;
    if (lowerName == L"end") return VK_END;
    if (lowerName == L"pageup") return VK_PRIOR;
    if (lowerName == L"pagedown") return VK_NEXT;
    if (lowerName == L"printscreen") return VK_SNAPSHOT;
    if (lowerName == L"scrolllock") return VK_SCROLL;
    if (lowerName == L"pause") return VK_PAUSE;
    // 6. 小键盘符号
    if (lowerName == L"numlock") return VK_NUMLOCK;
    if (lowerName == L"*") return VK_MULTIPLY;
    if (lowerName == L"+") return VK_ADD;
    if (lowerName == L"-") return VK_SUBTRACT;
    if (lowerName == L"/") return VK_DIVIDE;
    if (lowerName == L".") return VK_DECIMAL;
    // 未识别
    return 0;
}

void Util::setAutoStart(bool flag)
{
    std::wstring runKey = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";
    if (flag) {
        wchar_t buffer[MAX_PATH];
        GetModuleFileName(nullptr, buffer, MAX_PATH);
        auto curPath = std::filesystem::path(buffer);
        std::wstring commandLine = std::format(L"\"{}\" --auto-start", curPath.wstring());
        HKEY hKey;
        if (RegOpenKeyEx(HKEY_CURRENT_USER, runKey.data(), 0, KEY_WRITE, &hKey) == ERROR_SUCCESS) {
            RegSetValueEx(hKey, L"ScreenCapture", 0, REG_SZ, (const BYTE*)commandLine.data(), (commandLine.size() + 1) * sizeof(wchar_t));
            RegCloseKey(hKey);
        }
    }
    else {
        HKEY hKey;
        if (RegOpenKeyEx(HKEY_CURRENT_USER, runKey.data(), 0, KEY_WRITE, &hKey) == ERROR_SUCCESS) {
            RegDeleteValue(hKey, L"ScreenCapture");
            RegCloseKey(hKey);
        }
    }
}

std::wstring Util::getVer()
{
    std::vector<wchar_t> exePath(MAX_PATH);
    //获取exe文件的路径（自身路径）
    if (GetModuleFileName(nullptr, exePath.data(), static_cast<DWORD>(exePath.size())) == 0) {
        return L"0.0.0";
    }
    DWORD dummy;
    //获取版本资源大小
    DWORD versionSize = GetFileVersionInfoSize(exePath.data(), &dummy);
    if (versionSize == 0) {
        return L"0.0.0";
    }
    std::vector<BYTE> versionData(versionSize);
    //获取版本信息
    if (!GetFileVersionInfo(exePath.data(), 0, versionSize, versionData.data())) {
        return L"0.0.0";
    }
    VS_FIXEDFILEINFO* fileInfo = nullptr;
    UINT fileInfoSize = 0;
    if (!VerQueryValue(versionData.data(), L"\\", reinterpret_cast<void**>(&fileInfo), &fileInfoSize)) {
        return L"0.0.0";
    }
    int major = (fileInfo->dwFileVersionMS >> 16) & 0xFFFF;
    int minor = fileInfo->dwFileVersionMS & 0xFFFF;
    int patch = (fileInfo->dwFileVersionLS >> 16) & 0xFFFF;
    return std::format(L"{}.{}.{}",major,minor,patch);
}

std::wstring Util::getSysLang()
{
    LANGID langID = GetUserDefaultLangID(); //默认语言标识符
    WCHAR langName[LOCALE_NAME_MAX_LENGTH];
    auto areaId = MAKELCID(langID, SORT_DEFAULT); //区域设置ID
    if (LCIDToLocaleName(areaId, langName, LOCALE_NAME_MAX_LENGTH, 0)) {
        return std::wstring{ langName };
    }
    return L"zh-CN";
}
