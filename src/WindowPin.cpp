#include "WindowPin.h"
#include <windowsx.h>
#include "include/core/SkPath.h"
#include "include/core/SkRect.h"
#include "include/core/SkSurface.h"
#include "include/core/SkCanvas.h"
#include "include/utils/SkShadowUtils.h"
#include "include/core/SkPoint3.h"
#include "include/core/SkRRect.h"
#include "include/core/SkStream.h"
#include "include/core/SkImageFilter.h"
#include "include/encode/SkPngEncoder.h"
#include "App.h"
#include "Cursor.h"
#include "CutMask.h"
#include "ToolMain.h"
#include "ToolSub.h"
#include "Recorder.h"
#include "ColorBlender.h"
#include "WindowMain.h"


WindowPin::WindowPin()
{
    imgRect = CutMask::GetCutRect();
    auto windowMain = (WindowMain*)App::GetWin();
    windowMain->paintToDie();
    auto iRect = SkIRect::MakeLTRB(imgRect.fLeft, imgRect.fTop, imgRect.fRight, imgRect.fBottom);
    img = windowMain->surfaceBase->makeImageSnapshot(iRect);
    initSize();
    initWindow();
}

WindowPin::WindowPin(bool clipboard)
{
    OpenClipboard(NULL);
    HANDLE hData = GetClipboardData(CF_BITMAP);
    if (hData == NULL)
    {
        hData = GetClipboardData(CF_DIB);
        if (hData == NULL)
        {
            hData = GetClipboardData(CF_DIBV5);
        }
    }
    if (hData != NULL)
    {
        HBITMAP hBitmap = (HBITMAP)hData;
        BITMAP bitmap;
        GetObject(hBitmap, sizeof(BITMAP), &bitmap);
        long long rowBytes = bitmap.bmWidth * 4;
        long long dataSize = rowBytes * bitmap.bmHeight;
        std::vector<int32_t> buffer;
        buffer.resize(dataSize);
        HDC hScreen = GetDC(NULL);
        BITMAPINFO info = { sizeof(BITMAPINFOHEADER), (long)bitmap.bmWidth, 0 - (long)bitmap.bmHeight, 1, 32, BI_RGB, (DWORD)dataSize, 0, 0, 0, 0 };
        GetDIBits(hScreen, hBitmap, 0, bitmap.bmHeight, &buffer.front(), &info, DIB_RGB_COLORS);
        DeleteDC(hScreen);
        DeleteObject(hBitmap);
        ReleaseDC(NULL, hScreen);
        SkImageInfo imgInfo = SkImageInfo::MakeN32Premul(bitmap.bmWidth, bitmap.bmHeight);
        SkBitmap bm;
        bm.setInfo(imgInfo);
        bm.setPixels(&buffer.front());
        img = bm.asImage();
    }
    CloseClipboard();

    w = img->width() + shadowSize * 2;
    h = img->height() + shadowSize * 2 + ToolBtn::Height * 2 + ToolBase::MarginTop * 2;
    auto tmW = 15 * ToolBtn::Width;
    auto tempWidth = tmW + shadowSize * 2;
    if (w < tempWidth) {
        this->w = tempWidth;
    }

    RECT screenRect;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &screenRect, 0);
    x = (screenRect.right - w) / 2;
    y = (screenRect.bottom - h) / 2;    
    imgRect.setXYWH(shadowSize, shadowSize, img->width(), img->height());
    initWindow();
}

WindowPin::~WindowPin()
{
    ColorBlender::Reset();
}

void WindowPin::Save(const std::string& filePath)
{
    Recorder::Get()->FinishPaint();
    auto img = surfaceBase->makeImageSnapshot(SkIRect::MakeXYWH(shadowSize, shadowSize, surfaceFront->width(), surfaceFront->height()));    
    SkPixmap pixmap;
    img->peekPixels(&pixmap);
    SkPngEncoder::Options option;
    SkFILEWStream stream(filePath.data());
    SkPngEncoder::Encode(&stream, pixmap, option);
    stream.flush();
    App::Quit(6);
}

void WindowPin::SaveToClipboard()
{
    Recorder::Get()->FinishPaint();

    auto img = surfaceBase->makeImageSnapshot(SkIRect::MakeXYWH(shadowSize, shadowSize, surfaceFront->width(), surfaceFront->height()));
    int width{ (int)img->width() }, height{ (int)img->height() };
    SkPixmap pixmap;
    img->peekPixels(&pixmap);
    HDC screenDC = GetDC(nullptr);
    HDC memoryDC = CreateCompatibleDC(screenDC);
    HBITMAP hBitmap = CreateCompatibleBitmap(screenDC, width, height);
    DeleteObject(SelectObject(memoryDC, hBitmap));
    BITMAPINFO bitmapInfo = { 0 };
    bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmapInfo.bmiHeader.biWidth = width;
    bitmapInfo.bmiHeader.biHeight = -height;
    bitmapInfo.bmiHeader.biPlanes = 1;
    bitmapInfo.bmiHeader.biBitCount = 32;
    SetDIBitsToDevice(memoryDC, 0, 0, width, height, 0, 0, 0, height, pixmap.addr(), &bitmapInfo, DIB_RGB_COLORS);
    if (!OpenClipboard(nullptr)) {
        MessageBox(NULL, L"Failed to open clipboard when save to clipboard.", L"Error", MB_OK | MB_ICONERROR);
        return;
    }
    EmptyClipboard();
    SetClipboardData(CF_BITMAP, hBitmap);
    CloseClipboard();
    ReleaseDC(nullptr, screenDC);
    DeleteDC(memoryDC);
    DeleteObject(hBitmap);
    App::Quit(7);


    //HDC ScreenDC = GetDC(NULL);
    //HDC hMemDC = CreateCompatibleDC(ScreenDC);
    //auto w{ surfaceFront->width() }, h{ surfaceFront->height() };
    //HBITMAP hBitmap = CreateCompatibleBitmap(ScreenDC, w, h);
    //HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
    //StretchBlt(hMemDC, 0, 0, w, h, ScreenDC, x+shadowSize, y+shadowSize, w, h, SRCCOPY);
    //hBitmap = (HBITMAP)SelectObject(hMemDC, hOldBitmap);
    //DeleteDC(hMemDC);
    //DeleteObject(hOldBitmap);
    //if (!OpenClipboard(hwnd)) {
    //    MessageBox(NULL, L"Failed to open clipboard when save to clipboard.", L"Error", MB_OK | MB_ICONERROR);
    //    return;
    //}
    //EmptyClipboard();
    //SetClipboardData(CF_BITMAP, hBitmap);
    //CloseClipboard();
    //ReleaseDC(NULL, ScreenDC);
    //App::Quit(7);
}

void WindowPin::initCanvas()
{
    surfaceBase.reset();
    surfaceBack.reset();
    surfaceFront.reset();
    if (pixBase) {
        delete pixBase;
    }
    if (pixSrc) {
        delete pixSrc;
    }
    if (pixBack) {
        delete pixBack;
    }

    SkImageInfo imgInfo = SkImageInfo::MakeN32Premul(w, h);
    long long rowBytes = w * 4;
    long long dataSize = rowBytes * h;
    pixSrcData.resize(dataSize);    
    pixSrc = new SkPixmap(imgInfo, &pixSrcData.front(), rowBytes);
    auto canvas = SkCanvas::MakeRasterDirect(imgInfo, (void*)pixSrc->addr(), rowBytes);

    canvas->clear(SK_ColorTRANSPARENT);
    canvas->drawImageRect(img, imgRect, SkSamplingOptions(SkFilterMode::kNearest, SkMipmapMode::kNearest));

    SkPaint paint;
    SkPath path;
    path.addRRect(SkRRect::MakeRectXY(SkRect::MakeXYWH(shadowSize - 2, shadowSize - 2, imgRect.width() + 4, imgRect.height() + 4), 6, 6));
    SkPoint3 zPlaneParams = SkPoint3::Make(0, 0, 20);// 定义阴影与 z 平面的关系    
    SkPoint3 lightPos = SkPoint3::Make(0, 0, 0);// 定义光源的位置和半径
    SkShadowUtils::DrawShadow(canvas.get(), path, zPlaneParams, lightPos, 20.f, SkColorSetARGB(60, 0, 0, 0), SK_ColorTRANSPARENT, 0);    

    surfaceBase = SkSurfaces::Raster(imgInfo);
    pixBase = new SkPixmap();
    surfaceBase->peekPixels(pixBase);
    SkImageInfo info1 = SkImageInfo::MakeN32Premul(imgRect.width(), imgRect.height());
    pixBack = new SkPixmap();
    surfaceBack = SkSurfaces::Raster(info1);
    surfaceBack->peekPixels(pixBack);
    surfaceFront = SkSurfaces::Raster(info1);
}

void WindowPin::initSize()
{
    auto mainWin = App::GetWin();
    x = mainWin->x + imgRect.fLeft - shadowSize;
    y = mainWin->y + imgRect.fTop - shadowSize;
    w = imgRect.width() + shadowSize * 2;
    h = imgRect.height() + shadowSize * 2;
    auto tm = ToolMain::Get();
    tm->Btns[11]->IsDisable = true;
    auto tempWidth = tm->ToolRect.width() + shadowSize * 2;
    if (w < tempWidth) {
        this->w = tempWidth;
    }
    h += ToolBtn::Height * 2 + ToolBase::MarginTop * 2;
    imgRect.setXYWH(shadowSize, shadowSize, imgRect.width(), imgRect.height());
}

void WindowPin::showMenu()
{
    HMENU hMenu = CreatePopupMenu();
    AppendMenu(hMenu, state == State::start ? MF_UNCHECKED : MF_CHECKED, 1001, L"Tool(Ctrl+T)");
    AppendMenu(hMenu, MF_STRING, 1002, L"Exit(Esc)");
    POINT point;
    GetCursorPos(&point);
    TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, 0, hwnd, NULL);
}

void WindowPin::paintCanvas()
{
    surfaceBase->writePixels(*pixSrc, 0, 0);
    auto canvas = surfaceBase->getCanvas();
    
    auto imgTemp = surfaceBack->makeImageSnapshot();
    canvas->drawImage(imgTemp, shadowSize,shadowSize);
    imgTemp = surfaceFront->makeImageSnapshot();
    canvas->drawImage(imgTemp, shadowSize, shadowSize);
    ToolMain::Get()->OnPaint(canvas);
    ToolSub::Get()->OnPaint(canvas);
}

LRESULT WindowPin::wndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg)
    {
    case WM_LBUTTONDOWN:
    {
        IsMouseDown = true;
        auto x = GET_X_LPARAM(lparam);
        auto y = GET_Y_LPARAM(lparam);
        return onMouseDown(x, y);
    }
    case WM_LBUTTONUP:
    {
        IsMouseDown = false;
        auto x = GET_X_LPARAM(lparam);
        auto y = GET_Y_LPARAM(lparam);
        return onMouseUp(x, y);
    }
    case WM_MOUSEMOVE:
    {
        auto x = GET_X_LPARAM(lparam);
        auto y = GET_Y_LPARAM(lparam);
        if (IsMouseDown) {
            return onMouseDrag(x, y);
        }
        else {
            return onMouseMove(x, y);
        }
    }
    case WM_RBUTTONDOWN:
    {
        auto x = GET_X_LPARAM(lparam);
        auto y = GET_Y_LPARAM(lparam);
        onMouseDownRight(x, y);
        return true;
    }
    case WM_KEYDOWN:
    {
        onKeyDown(wparam);
        return false;
    }
    case WM_CHAR: {
        return onChar(wparam);
    }
    case WM_MOUSEWHEEL: {
        int delta = GET_WHEEL_DELTA_WPARAM(wparam);
        onMouseWheel(delta);
        return false;
    }
    case WM_COMMAND: {
        switch (LOWORD(wparam))
        {
        case 1001:
        {
            switchToolBar();
            break;
        }
        case 1002:
            App::Quit(1);
            break;
        }
        break;
    }
    case WM_TIMER: {
        return onTimeout(wparam);
    }
    default:
        break;
    }
    return DefWindowProc(hwnd, msg, wparam, lparam);
}

bool WindowPin::onMouseDown(const int& x, const int& y)
{
    if (state == State::start) {
        GetCursorPos(&startPos);
        SetCapture(hwnd);
    }
    else {
        if (ToolMain::Get()->OnMouseDown(x, y)) {
            return true;
        }
        if (ToolSub::Get()->OnMouseDown(x, y)) {
            return true;
        }
        Recorder::Get()->OnMouseDown(x - shadowSize, y - shadowSize);
    }
	return false;
}

bool WindowPin::onMouseDownRight(const int& x, const int& y)
{
    if (state == State::start) {
        showMenu();
    }
    else {
        auto flag = Recorder::Get()->OnMouseDownRight(x - shadowSize, y - shadowSize);
        if (!flag) {
            showMenu();
        }
    }    
	return false;
}

bool WindowPin::onMouseUp(const int& x, const int& y)
{
    if (state == State::start) {
        ReleaseCapture();
    }
    else {
        Recorder::Get()->OnMouseUp(x-shadowSize, y - shadowSize);
    }
	return false;
}

bool WindowPin::onMouseMove(const int& x, const int& y)
{
    if (state == State::start) {
        Cursor::Arrow();
        return false;
    }
    auto tm = ToolMain::Get()->OnMouseMove(x, y);
    auto ts = ToolSub::Get()->OnMouseMove(x, y);
    if (tm || ts) {
        return false;
    }
    Recorder::Get()->OnMouseMove(x - shadowSize, y - shadowSize);
    return false;
}

bool WindowPin::onMouseDrag(const int& x1, const int& y1)
{
    if (state == State::start) {
        POINT point;
        GetCursorPos(&point);
        int dx = point.x - startPos.x;
        int dy = point.y - startPos.y;
        x += dx;
        y += dy;
        SetWindowPos(hwnd, nullptr, x, y, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOSIZE);
        startPos = point;
    }
    else {
        Recorder::Get()->OnMouseDrag(x1 - shadowSize, y1 - shadowSize);
    }
	return false;
}

bool WindowPin::onChar(const unsigned int& val)
{
    Recorder::Get()->OnChar(val);
	return false;
}

bool WindowPin::onKeyDown(const unsigned int& val)
{
    bool flag = Recorder::Get()->OnKeyDown(val);
    if (flag) {
        return true;
    }
    if (val == VK_ESCAPE)
    {
        App::Quit(3);
    }
    else if (GetKeyState(VK_CONTROL) < 0) {
        if (val == 'C') {
            SaveToClipboard();
        }
        else if (val == 'S') {
            App::SaveFile();
        }
        else if (val == 'T') {
            switchToolBar();
        }
        else if (val == 'Y') {
            Recorder::Get()->Redo();
        }
        else if (val == 'Z') {
            Recorder::Get()->Undo();
        }
    }
	return true;
}

bool WindowPin::onMouseWheel(const int& delta)
{
    if (state == State::start) {
        float scale;
        if (delta > 0) {
            scale = 1.1;
        }
        else {
            scale = 0.9;
        }
        imgRect.setXYWH(shadowSize, shadowSize, imgRect.width() * scale, imgRect.height() * scale);
        auto tm = ToolMain::Get();
        float w1, h1;
        if (imgRect.width() < tm->ToolRect.width()) {
            w1 = tm->ToolRect.width() + shadowSize * 2;
        }
        else {
            w1 = imgRect.width() + shadowSize * 2;
        }
        h1 = imgRect.height() + shadowSize*2 + tm->ToolRect.height() * 2 + tm->MarginTop * 2;
        x = x - (w1 - w) / 2;
        y = y - (h1 - h) / 2;
        w = w1;
        h = h1;

        SetWindowPos(hwnd, nullptr, x, y, w, h, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
        DeleteDC(hCompatibleDC);
        DeleteObject(bottomHbitmap);
        HDC hdc = GetDC(hwnd);
        hCompatibleDC = CreateCompatibleDC(NULL);
        bottomHbitmap = CreateCompatibleBitmap(hdc, w, h);
        DeleteObject(SelectObject(hCompatibleDC, bottomHbitmap));
        ReleaseDC(hwnd, hdc);
        initCanvas();
        Refresh();
        return false;
    }
    Recorder::Get()->OnMouseWheel(delta);
	return false;
}
bool WindowPin::onTimeout(const unsigned int& id)
{
    Recorder::Get()->OnTimeout(id);
    return false;
}

void WindowPin::switchToolBar()
{
    if (state == State::start) {
        state = State::tool;
        auto tm = ToolMain::Get();
        tm->SetPosition(shadowSize, h - tm->ToolRect.height() * 2 - tm->MarginTop * 2);
        tm->UnSelectAndHoverAll();
    }
    else
    {
        state = State::start;
    }
    Refresh();
}
