#include "pch.h"
#include "Util.h"

void Util::createShadow(HWND hwnd)
{
    MARGINS margins = { 1,1,1,1 };
    DwmExtendFrameIntoClientArea(hwnd, &margins);
    int value = 2;
    DwmSetWindowAttribute(hwnd, DWMWA_NCRENDERING_POLICY, &value, sizeof(value));
    DwmSetWindowAttribute(hwnd, DWMWA_ALLOW_NCPAINT, &value, sizeof(value));
}

std::wstring Util::getResName(const std::wstring& url)
{
    size_t slashPos = url.find_last_of(L'/');
    size_t queryPos = url.find(L'?');
    size_t start = (slashPos != std::wstring::npos) ? slashPos + 1 : 0;
    size_t end = (queryPos != std::wstring::npos) ? queryPos : url.length();
    if (start >= end || start >= url.length()) return L"";
    return url.substr(start, end - start);
}

winrt::com_ptr<IStream> Util::getResStream(const std::wstring& resName)
{
    HRSRC hRes = FindResource(NULL, resName.data(), RT_RCDATA);
    if (!hRes) return nullptr;
    HGLOBAL hData = LoadResource(NULL, hRes);
    if (!hData) return nullptr;
    void* pData = LockResource(hData);
    DWORD size = SizeofResource(NULL, hRes);
    winrt::com_ptr<IStream> stream;
    stream.attach(SHCreateMemStream((const BYTE*)pData, size));
    return stream;
}

ID2D1Factory* Util::getD2D()
{
    static winrt::com_ptr<ID2D1Factory> D2D;
    if (!D2D.get()) {
        auto hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, D2D.put());
        if (FAILED(hr)) {
            throw std::runtime_error("D2D create error");
        }
    }
    return D2D.get();
}

IDWriteFactory5* Util::getWriteFactory()
{
    static winrt::com_ptr<IDWriteFactory5> dwriteFactory;
    if (!dwriteFactory.get()) {
        auto hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<::IUnknown**>(dwriteFactory.put()));
        if (FAILED(hr)) {
            throw std::runtime_error("dwriteFactory create error");
        }
    }
    return dwriteFactory.get();
}

winrt::com_ptr<ID2D1HwndRenderTarget> Util::createRender(HWND& hwnd, int& w, int& h)
{
    winrt::com_ptr<ID2D1HwndRenderTarget> render;
    D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_DEFAULT, //D2D1_RENDER_TARGET_TYPE_DEFAULT,//D2D1_RENDER_TARGET_TYPE_HARDWARE,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
        96, 96
    );
    D2D1_SIZE_U size = D2D1::SizeU(w, h);
    D2D1_HWND_RENDER_TARGET_PROPERTIES hwndProps = D2D1::HwndRenderTargetProperties(hwnd, size);
    auto d2d = Util::getD2D();
    auto hr = d2d->CreateHwndRenderTarget(rtProps, hwndProps, render.put());
    if (FAILED(hr)) {
        throw std::runtime_error("CreateHwndRenderTarget error");
    }
    return render;
}

IDWriteTextFormat* Util::getTextFormat(const float& fontSize)
{
    static winrt::com_ptr<IDWriteTextFormat> textFormat;
    if (!textFormat.get()) {
        auto dwriteFactory = Util::getWriteFactory();
        dwriteFactory->CreateTextFormat(L"Microsoft YaHei", nullptr,
            DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
            fontSize, L"", textFormat.put());
    }
    return textFormat.get();
}

IDWriteTextFormat* Util::getIconFormat()
{
    static winrt::com_ptr<IDWriteTextFormat> iconFormat;
    if (!iconFormat.get()) {
        HRSRC hRes = FindResource(NULL, L"iconfont.ttf", RT_RCDATA);
        if (!hRes) return nullptr;
        HGLOBAL hData = LoadResource(NULL, hRes);
        if (!hData) return nullptr;
        void* pData = LockResource(hData);
        DWORD size = SizeofResource(NULL, hRes);

        auto dwriteFactory = Util::getWriteFactory();
        winrt::com_ptr<IDWriteInMemoryFontFileLoader> loader;
        dwriteFactory->CreateInMemoryFontFileLoader(loader.put());
        dwriteFactory->RegisterFontFileLoader(loader.get());
        winrt::com_ptr<IDWriteFontFile> fontFile;
        loader->CreateInMemoryFontFileReference(dwriteFactory, pData, size, nullptr, fontFile.put());
        winrt::com_ptr<IDWriteFontSetBuilder1> fontSetBuilder;
        dwriteFactory->CreateFontSetBuilder(fontSetBuilder.put());
        fontSetBuilder->AddFontFile(fontFile.get());
        winrt::com_ptr<IDWriteFontSet> fontSet;
        fontSetBuilder->CreateFontSet(fontSet.put());
        winrt::com_ptr<IDWriteFontCollection1> fontCollection;
        dwriteFactory->CreateFontCollectionFromFontSet(fontSet.get(), fontCollection.put());
        //auto win = WinCap::get();
        dwriteFactory->CreateTextFormat(L"icon", fontCollection.get(),
            DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
            12.f, L"", iconFormat.put());

        //ComPtr<IDWriteTextLayout> layout;
        //dwriteFactory->CreateTextLayout(L"asdf", wcslen(L"asdf"), iconFormat.Get(),
        //    1000, 800, layout.GetAddressOf());
        //layout->SetFontSize(1212.f, { 0, 0 });
        //ComPtr<ID2D1SolidColorBrush> brush;
        //rt->CreateSolidColorBrush(fontColor, brush.GetAddressOf());
        //// 2. 在哪画、怎么画
        //D2D1_POINT_2F origin = { 10.0f, 20.0f }; // 左上角起点（逻辑坐标）
        //rt->DrawTextLayout(
        //    origin,
        //    layout.Get(),
        //    brush.Get(),
        //    D2D1_DRAW_TEXT_OPTIONS_NONE // 或 ENABLE_COLOR_FONT / CLIP 等
        //);
    }

    return iconFormat.get();
}

winrt::com_ptr<IDWriteTextLayout> Util::getIconLayout(const std::wstring& icon, const float& fontSize, const float& w, const float& h)
{
    auto format = Util::getIconFormat();
    auto dwrite = Util::getWriteFactory();
    winrt::com_ptr<IDWriteTextLayout> layout;
    dwrite->CreateTextLayout(icon.data(), icon.length(), format, w, h, layout.put());
    layout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    layout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    layout->SetFontSize(fontSize, { 0, static_cast<UINT32>(icon.length()) });
    return layout;
}

winrt::com_ptr<IDWriteTextLayout> Util::getTextLayout(const std::wstring& str, const float& fontSize, const float& w, const float& h)
{
    auto format = Util::getIconFormat();
    auto dwrite = Util::getWriteFactory();
    winrt::com_ptr<IDWriteTextLayout> layout;
    dwrite->CreateTextLayout(str.data(), str.length(), format, w, h, layout.put());
    layout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
    layout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    layout->SetFontSize(fontSize, { 0, static_cast<UINT32>(str.length()) });
    return layout;
}

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

winrt::com_ptr<ID2D1PathGeometry> Util::createArrow(const D2D1_POINT_2F& startPos, const D2D1_POINT_2F& endPos)
{
    auto factory = Util::getD2D();
    winrt::com_ptr<ID2D1PathGeometry> arrow;
    factory->CreatePathGeometry(arrow.put());
    winrt::com_ptr<ID2D1GeometrySink> sink;
    arrow->Open(sink.put());
    sink->BeginFigure(startPos, D2D1_FIGURE_BEGIN_FILLED);
    float dx = endPos.x - startPos.x;
    float dy = endPos.y - startPos.y;
    float arrowSize{ 20.f };
    float length = sqrtf(dx * dx + dy * dy);
    // 单位方向向量
    float ux = dx / length;
    float uy = dy / length;
    // 单位垂直向量（向左）
    float vx = -uy;
    float vy = ux;
    float v1 = arrowSize / 4.0f;
    float v2 = arrowSize * 2.0f / 3.0f;
    // P1: 箭杆末端内侧左点 = 终点 - arrowSize * 方向 + (-v1) * 垂直
    sink->AddLine({ endPos.x - arrowSize * ux - v1 * vx ,endPos.y - arrowSize * uy - v1 * vy });
    // P2: 箭头左尖 = P1 - v1 * 方向 - (v2 - v1) * 垂直？  
    // 更直观：从终点往回走 (arrowSize + v1) 沿方向，再偏移 v2 左
    sink->AddLine({ endPos.x - (arrowSize + v1) * ux - v2 * vx,endPos.y - (arrowSize + v1) * uy - v2 * vy });
    // P3: 箭头顶点 = 终点
    sink->AddLine(endPos);
    // P4: 箭头右尖 = 终点往回走 (arrowSize + v1) 沿方向，再偏移 v2 右（+v2 * 垂直）
    sink->AddLine({ endPos.x - (arrowSize + v1) * ux + v2 * vx ,endPos.y - (arrowSize + v1) * uy + v2 * vy });
    // P5: 箭杆末端内侧右点 = 终点 - arrowSize * 方向 + v1 * 垂直
    sink->AddLine({ endPos.x - arrowSize * ux + v1 * vx ,endPos.y - arrowSize * uy + v1 * vy });
    sink->EndFigure(D2D1_FIGURE_END_CLOSED);
    sink->Close();
    return arrow;
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

void Util::enableHwndAlpha(HWND hwnd)
{
    HRGN region = CreateRectRgn(0, 0, -1, -1);
    DWM_BLURBEHIND bb = { 0 };
    bb.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
    bb.hRgnBlur = region;
    bb.fEnable = TRUE;
    DwmEnableBlurBehindWindow(hwnd, &bb);
    DeleteObject(region);
}
