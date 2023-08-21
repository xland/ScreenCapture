#include "MainWin.h"
#include "dwmapi.h"
#include "resource.h"
#include <Commctrl.h>
#include <Shobjidl.h> 
#include <atlbase.h>
#include <Gdiplus.h> 
#include <format>
#include <string>
#include <sstream>

/// <summary>
/// 0 undefined
/// 1 quit by press close btn;
/// 2 quit by press right mouse btn;
/// 3 quit by press esc keyboard
/// 4 quit when copy rgb color
/// 5 quit when copy hex color
/// 6 quit when save to file
/// 7 quit when save to clipboard
/// </summary>
static int mainWinQuitCode = 0;


void MainWin::initLayerImg() {
    x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    h = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    HDC hScreen = GetDC(NULL);
    HDC hDC = CreateCompatibleDC(hScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, w, h);
    DeleteObject(SelectObject(hDC, hBitmap));
    BOOL bRet = BitBlt(hDC, 0, 0, w, h, hScreen, x, y, SRCCOPY);
    unsigned int stride = w * 4;
    unsigned int dataSize = stride * h;
    auto desktopPixelData = new unsigned char[dataSize];
    BITMAPINFO info = { sizeof(BITMAPINFOHEADER), (long)w, 0 - (long)h, 1, 32, BI_RGB, dataSize, 0, 0, 0, 0 };
    GetDIBits(hDC, hBitmap, 0, h, desktopPixelData, &info, DIB_RGB_COLORS);
    DeleteDC(hDC);
    DeleteObject(hBitmap);

    PaintCtx = new BLContext();
    desktopImage = new BLImage();
    desktopImage->createFromData(w, h, BL_FORMAT_PRGB32, desktopPixelData, stride, BL_DATA_ACCESS_RW, [](void* impl, void* externalData, void* userData) {
        delete[] externalData;
        });
    canvasImage = new BLImage(w, h, BL_FORMAT_PRGB32);
    prepareImage = new BLImage(w, h, BL_FORMAT_PRGB32);
    PaintCtx->begin(*prepareImage);
    PaintCtx->clearAll();
    PaintCtx->end();
    PaintCtx->begin(*canvasImage);
    PaintCtx->clearAll();
    PaintCtx->end();


    BottomImage = new BLImage(w, h, BL_FORMAT_PRGB32);
    BLImageData imgData;
    BottomImage->getData(&imgData);
    pixelData = (unsigned char*)imgData.pixelData;
}

bool MainWin::OnPaint()
{
    PaintCtx->begin(*BottomImage);
    PaintCtx->blitImage(BLRect(0, 0, w, h), *desktopImage);
    if (IsMosaicUsePen) {
        PaintCtx->blitImage(BLRect(0, 0, w, h), *mosaicImage);
    }
    else
    {
        PaintCtx->blitImage(BLRect(0, 0, w, h), *canvasImage);
    }
    if (IsDrawing) {
        PaintCtx->blitImage(BLRect(0, 0, w, h), *prepareImage);
    }
    PaintCtx->setFillStyle(BLRgba32(0, 0, 0, 180));
    PaintCtx->fillBoxArray(maskBoxes, 8);
    PaintCtx->setStrokeStyle(BLRgba32(22, 119, 255));
    PaintCtx->setStrokeWidth(cutBoxBorderWidth);
    PaintCtx->strokeBox(cutBox);
    drawToolMain();
    if (!IsLeftBtnDown && state == State::start) {
        drawPixelInfo();
    }
    PaintCtx->end();   
    return true;
}


std::wstring MainWin::getPixelColorRgb()
{
    size_t pixelIndex = pixelY * w * 4 + pixelX * 4;
    auto result = std::to_wstring(pixelData[pixelIndex + 2]) + L"," +
        std::to_wstring(pixelData[pixelIndex + 1]) + L"," +
        std::to_wstring(pixelData[pixelIndex]);
    return result;
}
std::wstring MainWin::getPixelColorHex()
{
    size_t pixelIndex = pixelY * w * 4 + pixelX * 4;
    std::wstringstream ss;
    int rgb = ((pixelData[pixelIndex + 2] << 16) | (pixelData[pixelIndex + 1] << 8) | pixelData[pixelIndex]);
    ss << std::hex << rgb;
    std::wstring hexColorStr = ss.str();
    int str_length = hexColorStr.length();
    for (int i = 0; i < 6 - str_length; i++) {
        hexColorStr = L"0" + hexColorStr;
    }
    return L"#" + hexColorStr;
}

void MainWin::drawPixelInfo()
{
    if (pixelY == -999999 && pixelX == -999999) {
        return;
    }
    BLRectI rectDst;
    rectDst.x = pixelX + 10;
    rectDst.y = pixelY + 10;
    rectDst.w = 200;
    rectDst.h = 100;
    if (rectDst.x + rectDst.w > w) {
        rectDst.x -= (rectDst.w + 20);
    }
    if (rectDst.y + rectDst.w > h) {
        rectDst.y -= (rectDst.w + 20);
    }
    {
        auto srcX = pixelX - 10;
        auto srcY = pixelY - 5;
        auto dstX = rectDst.x;
        auto dstY = rectDst.y;
        static size_t dataSize = w * 4 * h;
        for (size_t y = 0; y < 10; y++)
        {
            for (size_t x = 0; x < 20; x++) {
                static BLRgba32 color;
                if (srcX < 0 || srcY < 0 || srcX >= w || srcY >= h) {
                    color.setB(0);
                    color.setG(0);
                    color.setR(0);
                    color.setA(255);
                }
                else
                {
                    size_t indexSrc = srcY * w * 4 + srcX * 4;
                    color.setB(pixelData[indexSrc]);
                    color.setG(pixelData[indexSrc + 1]);
                    color.setR(pixelData[indexSrc + 2]);
                    color.setA(255);
                }
                PaintCtx->setFillStyle(color);
                PaintCtx->fillRect(dstX, dstY, 10, 10);
                dstX += 10;
                srcX += 1;
            }
            dstX = rectDst.x;
            dstY += 10;
            srcX = pixelX - 10;
            srcY += 1;
        }
    }
    PaintCtx->setStrokeStyle(BLRgba32(0, 0, 0));
    PaintCtx->setStrokeWidth(1);
    PaintCtx->strokeRect(BLRectI(rectDst.x, rectDst.y, rectDst.w, rectDst.h * 2));
    {
        int halfCrossW = 5;
        int halfW = rectDst.w / 2;
        int halfH = rectDst.h / 2;
        PaintCtx->setStrokeStyle(BLRgba32(22, 119, 255, 110));
        PaintCtx->setStrokeWidth(halfCrossW * 2);
        PaintCtx->strokeLine(rectDst.x, rectDst.y + halfH, rectDst.x + halfW - halfCrossW, rectDst.y + halfH);
        PaintCtx->strokeLine(rectDst.x + halfW + halfCrossW, rectDst.y + halfH, rectDst.x + rectDst.w, rectDst.y + halfH);
        PaintCtx->strokeLine(rectDst.x + halfW, rectDst.y, rectDst.x + halfW, rectDst.y + halfH - halfCrossW);
        PaintCtx->strokeLine(rectDst.x + halfW, rectDst.y + halfH + halfCrossW, rectDst.x + halfW, rectDst.y + rectDst.h);
    }
    PaintCtx->setFillStyle(BLRgba32(0, 0, 0, 200));
    PaintCtx->fillRect(BLRectI(rectDst.x, rectDst.y + rectDst.h, rectDst.w, rectDst.h));

    auto font = Font::Get()->fontText;
    font->setSize(16.6);
    PaintCtx->setFillStyle(BLRgba32(255, 255, 255));
    auto textX = rectDst.x + 8;
    auto textY = rectDst.y + rectDst.h + 14 + font->metrics().ascent;
    std::string utf8 = std::format("Position:X:{} Y:{}", pixelX, pixelY);
    PaintCtx->fillUtf8Text(BLPoint(textX, textY), *font, utf8.data());
    size_t pixelIndex = pixelY * w * 4 + pixelX * 4;
    utf8 = std::format("RGB(Ctr+R):{},{},{}", pixelData[pixelIndex + 2], pixelData[pixelIndex + 1], pixelData[pixelIndex]);
    PaintCtx->fillUtf8Text(BLPoint(textX, textY + 28), *font, utf8.data());
    std::stringstream ss;
    ss << std::hex << (pixelData[pixelIndex + 2] << 16 | pixelData[pixelIndex + 1] << 8 | pixelData[pixelIndex]);
    std::string hex = ss.str();
    size_t str_length = hex.length();
    for (size_t i = 0; i < 6 - str_length; i++) {
        hex = "0" + hex;
    }
    utf8 = std::format("HEX(Ctr+H):#{}", hex);
    PaintCtx->fillUtf8Text(BLPoint(textX, textY + 56), *font, utf8.data());
}


bool MainWin::OnTimer(const unsigned int& id)
{
    if (id == 999) {
        auto shape = dynamic_cast<Shape::Text*>(History::GetLastDrawShape());
        if (shape) {
            shape->Draw(-1, -1, -1, -1);
        }
    }
    else if (id == 998) {
        saveClipboard();
    }
    return true;
}

bool MainWin::OnKeyDown(const unsigned int& key) {
    switch (key)
    {
        case VK_DELETE: {
            auto shape = dynamic_cast<Shape::Text*>(History::GetLastDrawShape());
            if (!shape) return false;
            shape->DeleteWord(false);
            return false;
        }
        case VK_LEFT: {
            auto shape = dynamic_cast<Shape::Text*>(History::GetLastDrawShape());
            if (!shape) return false;
            if (shape->moveCursorIndex(true)) {
                Refresh();
            }
            return false;
        }
        case VK_RIGHT: {
            auto shape = dynamic_cast<Shape::Text*>(History::GetLastDrawShape());
            if (!shape) return false;
            if (shape->moveCursorIndex(false)) {
                Refresh();
            }
            return false;
        }
        case VK_ESCAPE: {
            quitApp(3);
            return false;
        }
        case VK_SHIFT: {
            IsShiftDown = true;
            return false;
        }
        case VK_CONTROL: {
            IsCtrlDown = true;
            return false;
        }
        case 82: { //R
            if (IsCtrlDown) {
                SetClipboardText(hwnd, getPixelColorRgb());
                quitApp(4);
                return true;
            }
            return false;
        }
        case 72: { //H
            if (IsCtrlDown) {
                SetClipboardText(hwnd, getPixelColorHex());
                quitApp(5);
                return true;
            }
            return false;
        }
        case 90: { //Z
            if (IsCtrlDown) {
                mouseEnterMainToolIndex = 9;
                History::Undo();
                return true;
            }
            return false;
        }
        case 89: { //Y
            if (IsCtrlDown) {
                mouseEnterMainToolIndex = 10;
                History::Redo();
                return true;
            }
            return false;
        }
    }
    return false;
}

bool MainWin::OnKeyUp(const unsigned int& key)
{
    switch (key)
    {
        case VK_SHIFT: {
            IsShiftDown = false;
        
        }
        case VK_CONTROL: {
            IsCtrlDown = false;
        }
    }
    return false;
}

//LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
//{
//    switch (msg)
//    {
//        case WM_LBUTTONDBLCLK:
//        {
//            saveClipboard();
//            return 0;
//        }
//        case WM_DESTROY:
//        {
//            PostQuitMessage(0);
//            return 1;
//        }        
//        case WM_CHAR:
//        {
//            if (wParam == 13) { //enter
//                History::LastShapeDrawEnd();
//                return 1;
//            }
//            auto shape = dynamic_cast<Shape::Text*>(History::GetLastDrawShape());
//            if (!shape) return 0;
//            if (wParam == 8) {
//                shape->DeleteWord();
//            }
//            else
//            {
//                shape->InsertWord(std::wstring{(wchar_t)wParam});
//            } 
//            return 1;
//        }        
//    }
//    return DefWindowProc(hWnd, msg, wParam, lParam);
//}


void MainWin::saveFile()
{
    SYSTEMTIME localTime;
    GetLocalTime(&localTime);
    std::wstring name = std::to_wstring(localTime.wYear)+ 
        std::to_wstring(localTime.wMonth) + 
        std::to_wstring(localTime.wDay) + 
        std::to_wstring(localTime.wHour) + 
        std::to_wstring(localTime.wMinute) + 
        std::to_wstring(localTime.wSecond) + 
        std::to_wstring(localTime.wMilliseconds);
    ATL::CComPtr<IFileSaveDialog> saveDialog;
    auto result = saveDialog.CoCreateInstance(CLSID_FileSaveDialog);
    saveDialog->SetDefaultExtension(L"png");
    saveDialog->SetFileName(name.c_str());
    saveDialog->SetTitle(L"保存文件");
    //saveDialog->SetFilter(L"Image\0*.png\0");
    if (FAILED(result)) return;
    result = saveDialog->SetOptions(FOS_FORCEFILESYSTEM | FOS_PATHMUSTEXIST | FOS_OVERWRITEPROMPT);
    if (FAILED(result)) return;
    result = saveDialog->Show(hwnd);
    if (result == HRESULT_FROM_WIN32(ERROR_CANCELLED)) {
        //用户取消
        return;
    }
    if (FAILED(result)) return;
    CComPtr<IShellItem> item;
    result = saveDialog->GetResult(&item);
    if (FAILED(result)) return;
    LPWSTR filePath;
    item->GetDisplayName(SIGDN_FILESYSPATH, &filePath);
    auto w = cutBox.x1 - cutBox.x0;
    auto h = cutBox.y1 - cutBox.y0;
    BLImage imgSave(w, h, BL_FORMAT_PRGB32);
    PaintCtx->begin(imgSave);
    PaintCtx->blitImage(BLPoint(0,0), *desktopImage, BLRectI((int)cutBox.x0, (int)cutBox.y0, (int)w, (int)h));
    PaintCtx->blitImage(BLPoint(0,0), *canvasImage, BLRectI((int)cutBox.x0, (int)cutBox.y0, (int)w, (int)h));
    PaintCtx->end();
    auto filePathUtf8 = ConvertToUTF8(filePath);
    imgSave.writeToFile(filePathUtf8.c_str());
    CoTaskMemFree(filePath);
    quitApp(6);
}
void MainWin::saveClipboard()
{
    int x = this->x + (int)cutBox.x0 + cutBoxBorderWidth / 2;
    int y = this->y + (int)cutBox.y0 + cutBoxBorderWidth / 2;
    int w = (int)cutBox.x1 - (int)cutBox.x0- cutBoxBorderWidth;
    int h = (int)cutBox.y1 - (int)cutBox.y0- cutBoxBorderWidth;
    HDC ScreenDC = ::GetDC(NULL);
    HDC hMemDC = CreateCompatibleDC(ScreenDC);
    HBITMAP hBitmap = CreateCompatibleBitmap(ScreenDC, w, h);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
    StretchBlt(hMemDC, 0, 0, w, h, ScreenDC, x, y, w, h, SRCCOPY);
    hBitmap = (HBITMAP)SelectObject(hMemDC, hOldBitmap);
    DeleteDC(hMemDC);
    DeleteObject(hOldBitmap);
    OpenClipboard(hwnd);
    EmptyClipboard();
    SetClipboardData(CF_BITMAP, hBitmap);
    CloseClipboard();
    ReleaseDC(NULL,ScreenDC);
    quitApp(7);
}

void MainWin::initWindowBoxes()
{
    EnumWindows([](HWND hwnd, LPARAM lparam)
        {
            if (!hwnd) return TRUE;
            if (!IsWindowVisible(hwnd)) return TRUE;
            if (IsIconic(hwnd)) return TRUE;
            if (GetWindowTextLength(hwnd) < 1) return TRUE;
            auto self = (MainWin*)lparam;
            RECT rect;
            GetWindowRect(hwnd, &rect);
            if (rect.right - rect.left <= 6 || rect.bottom - rect.top <= 6) {
                return TRUE;
            }
            BLBox item(rect.left - self->x,rect.top - self->y,
                rect.right - self->x,rect.bottom - self->y);
            self->windowBoxes.push_back(std::move(item));
            return TRUE;
        }, (LPARAM)this);
}
void MainWin::quitApp(const int& exitCode)
{
    mainWinQuitCode = exitCode;
    CloseWindow(hwnd);
    PostQuitMessage(0);
}
int MainWin::GetQuitCode()
{
    return mainWinQuitCode;
}