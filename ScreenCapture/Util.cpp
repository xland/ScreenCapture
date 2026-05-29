#include "pch.h"
#include "Util.h"

ID2D1Factory* Util::getD2D()
{
    static ComPtr<ID2D1Factory> D2D;
    if (!D2D.Get()) {
        auto hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, D2D.GetAddressOf());
        if (FAILED(hr)) {
            throw std::runtime_error("D2D create error");
        }
    }
    return D2D.Get();
}

IDWriteFactory5* Util::getWriteFactory()
{
    static ComPtr<IDWriteFactory5> dwriteFactory;
    if (!dwriteFactory.Get()) {
        auto hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<::IUnknown**>(dwriteFactory.GetAddressOf()));
        if (FAILED(hr)) {
            throw std::runtime_error("dwriteFactory create error");
        }
    }
    return dwriteFactory.Get();
}

IDWriteTextFormat* Util::getTextFormat(const float& fontSize)
{
    static ComPtr<IDWriteTextFormat> textFormat;
    if (!textFormat.Get()) {
        auto dwriteFactory = Util::getWriteFactory();
        dwriteFactory->CreateTextFormat(L"Microsoft YaHei", nullptr,
            DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
            fontSize, L"", textFormat.GetAddressOf());
    }
    return textFormat.Get();
}

IDWriteTextFormat* Util::getIconFormat()
{
    static ComPtr<IDWriteTextFormat> iconFormat;
    if (!iconFormat.Get()) {
        HRSRC hRes = FindResource(NULL, L"iconfont.ttf", RT_RCDATA);
        if (!hRes) return nullptr;
        HGLOBAL hData = LoadResource(NULL, hRes);
        if (!hData) return nullptr;
        void* pData = LockResource(hData);
        DWORD size = SizeofResource(NULL, hRes);

        auto dwriteFactory = Util::getWriteFactory();
        ComPtr<IDWriteInMemoryFontFileLoader> loader;
        dwriteFactory->CreateInMemoryFontFileLoader(loader.GetAddressOf());
        dwriteFactory->RegisterFontFileLoader(loader.Get());
        ComPtr<IDWriteFontFile> fontFile;
        loader->CreateInMemoryFontFileReference(dwriteFactory, pData, size, nullptr, fontFile.GetAddressOf());
        ComPtr<IDWriteFontSetBuilder1> fontSetBuilder;
        dwriteFactory->CreateFontSetBuilder(fontSetBuilder.GetAddressOf());
        fontSetBuilder->AddFontFile(fontFile.Get());
        ComPtr<IDWriteFontSet> fontSet;
        fontSetBuilder->CreateFontSet(fontSet.GetAddressOf());
        ComPtr<IDWriteFontCollection1> fontCollection;
        dwriteFactory->CreateFontCollectionFromFontSet(fontSet.Get(), fontCollection.GetAddressOf());
        //auto win = WinCap::get();
        dwriteFactory->CreateTextFormat(L"icon", fontCollection.Get(),
            DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
            12.f, L"", iconFormat.GetAddressOf());

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

    return iconFormat.Get();
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

ComPtr<ID2D1PathGeometry> Util::createArrow(const D2D1_POINT_2F& startPos, const D2D1_POINT_2F& endPos)
{
    auto factory = Util::getD2D();
    ComPtr<ID2D1PathGeometry> arrow;
    factory->CreatePathGeometry(arrow.GetAddressOf());
    ComPtr<ID2D1GeometrySink> sink;
    arrow->Open(sink.GetAddressOf());
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

ID2D1StrokeStyle* Util::getRoundStrokeStyle()
{
    static ComPtr<ID2D1StrokeStyle> roundStrokeStyle;
    if (!roundStrokeStyle.Get()) {
        auto d2d = getD2D();
        d2d->CreateStrokeStyle(
            D2D1::StrokeStyleProperties(
                D2D1_CAP_STYLE_ROUND,    // 起点线帽：圆角
                D2D1_CAP_STYLE_ROUND,    // 终点线帽：圆角
                D2D1_CAP_STYLE_ROUND,    // 虚线端点（如有）
                D2D1_LINE_JOIN_ROUND,    // 线段连接处：圆角
                8.f,                     // miterLimit
                D2D1_DASH_STYLE_SOLID,
                0.0f
            ),
            nullptr,
            0,
            roundStrokeStyle.GetAddressOf()
        );
    }
    return roundStrokeStyle.Get();
}
