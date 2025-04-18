#include <QMessageBox>
#include <QFont>
#include <QFontDatabase>
#include <ShellScalingApi.h>

#include "Util.h"
#include "NativeRect.h"

QFont* Util::getIconFont(const int& fontSize)
{
    static QFont font = []() {
        int fontId = QFontDatabase::addApplicationFont(":/Res/iconfont.ttf");
        QString fontName = QFontDatabase::applicationFontFamilies(fontId).at(0);
        QFont iconFont(fontName);
        iconFont.setStyleStrategy(QFont::PreferAntialias);
        iconFont.setHintingPreference(QFont::PreferNoHinting);
        return iconFont;
        }();
    font.setPixelSize(fontSize);
    return &font;
}

QFont* Util::getTextFont(const int& fontSize)
{
    static QFont font = []() {
        QFont font("Microsoft YaHei");
        font.setStyleStrategy(QFont::PreferAntialias);
        font.setHintingPreference(QFont::PreferNoHinting);
        return font;
        }();
    font.setPixelSize(fontSize);
    return &font;
}

QMouseEvent Util::createMouseEvent(const QEvent::Type& type, const Qt::MouseButton& btn)
{
    POINT pos; 
    GetCursorPos(&pos);
    auto isCtrl = GetKeyState(VK_CONTROL) & 0x8000;
    auto isShift = GetKeyState(VK_SHIFT) & 0x8000;
    Qt::KeyboardModifiers mf = Qt::NoModifier;
    if (isCtrl) mf = Qt::ControlModifier;
    if (isShift) mf = mf | Qt::ShiftModifier;
    QPointF p(pos.x, pos.y);
    QPointF g(pos.x, pos.y);
    return QMouseEvent(type, p, g, btn, btn, mf);
}

QMouseEvent Util::createMouseEvent(const LPARAM& lParam, const QEvent::Type& type, const Qt::MouseButton& btn)
{
    auto x = GET_X_LPARAM(lParam);
    auto y = GET_Y_LPARAM(lParam); 
    auto isCtrl = GetKeyState(VK_CONTROL) & 0x8000;
    auto isShift = GetKeyState(VK_SHIFT) & 0x8000;
    Qt::KeyboardModifiers mf = Qt::NoModifier;
    if (isCtrl) mf = Qt::ControlModifier;
    if (isShift) mf = mf | Qt::ShiftModifier;
    QPointF p(x, y);
    POINT pos;
    GetCursorPos(&pos);  //比ClientToScreen 效率要高
    QPointF g(pos.x, pos.y);
    return QMouseEvent(type, p, g, btn, btn, mf);
}
QImage Util::printScreen(const int& x, const int& y, const int& w, const int& h)
{
    HDC hScreen = GetDC(NULL);
    HDC hDC = CreateCompatibleDC(hScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, w, h);
    DeleteObject(SelectObject(hDC, hBitmap));
    BOOL bRet = BitBlt(hDC, 0, 0, w, h, hScreen, x, y, SRCCOPY);
    auto img = QImage(w, h, QImage::Format_ARGB32_Premultiplied);
    BITMAPINFO bmi = { sizeof(BITMAPINFOHEADER), (long)w, 0 - (long)h, 1, 32, BI_RGB, (DWORD)w * 4 * h, 0, 0, 0, 0 };
    GetDIBits(hDC, hBitmap, 0, h, img.bits(), &bmi, DIB_RGB_COLORS);
    DeleteDC(hDC);
    DeleteObject(hBitmap);
    ReleaseDC(NULL, hScreen);
    return img;
}
QImage Util::printScreen()
{
    auto rect = NativeRect::getDesktopRect();
    auto x = rect.x(), y = rect.y();
    auto w = rect.width(), h = rect.height();
    HDC hScreen = GetDC(NULL);
    HDC hDC = CreateCompatibleDC(hScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, w, h);
    DeleteObject(SelectObject(hDC, hBitmap));
    BOOL bRet = BitBlt(hDC, 0, 0, w, h, hScreen, x, y, SRCCOPY);
    auto img = QImage(w, h, QImage::Format_ARGB32_Premultiplied);
    BITMAPINFO bmi = { sizeof(BITMAPINFOHEADER), (long)w, 0 - (long)h, 1, 32, BI_RGB, (DWORD)w * 4 * h, 0, 0, 0, 0 };
    GetDIBits(hDC, hBitmap, 0, h, img.bits(), &bmi, DIB_RGB_COLORS);
    DeleteDC(hDC);
    DeleteObject(hBitmap);
    ReleaseDC(NULL, hScreen);
    return img;
}
void Util::imgToClipboard(const QImage& img)
{
    auto width = img.width();
    auto height = img.height();
    HDC screenDC = GetDC(nullptr);
    HDC memoryDC = CreateCompatibleDC(screenDC);
    HBITMAP hBitmap = CreateCompatibleBitmap(screenDC, width, height);
    DeleteObject(SelectObject(memoryDC, hBitmap));
    BITMAPINFO bmi = { sizeof(BITMAPINFOHEADER), width, 0 - height, 1, 32, BI_RGB, width * 4 * height, 0, 0, 0, 0 };
    SetDIBitsToDevice(memoryDC, 0, 0, width, height, 0, 0, 0, height, img.bits(), &bmi, DIB_RGB_COLORS);
    if (!OpenClipboard(nullptr)) {
        QMessageBox::warning(NULL, "Error", "Failed to open clipboard when save to clipboard.", QMessageBox::StandardButton::Ok);
        return;
    }
    EmptyClipboard();
    SetClipboardData(CF_BITMAP, hBitmap);
    CloseClipboard();
    ReleaseDC(nullptr, screenDC);
    DeleteDC(memoryDC);
    DeleteObject(hBitmap);
}
bool Util::posInScreen(const int& x, const int& y)
{
    QList<QScreen*> screens = QGuiApplication::screens();
    for (QScreen* screen : screens) {
        if (screen->geometry().contains(x, y)) {
            return true;
        }
    }
    return false;
}
QScreen* Util::getScreen(const int& x, const int& y)
{
    QList<QScreen*> screens = QGuiApplication::screens();
    for (QScreen* screen : screens) {
        if (screen->geometry().contains(x, y)) {
            return screen;
        }
    }
    return nullptr;
}
void Util::setClipboardText(const std::wstring& text) {
    OpenClipboard(NULL);
    EmptyClipboard();
    size_t len = (text.size() + 1) * sizeof(wchar_t);
    HANDLE copyHandle = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, len);
    if (copyHandle == NULL)
    {
        MessageBox(NULL, L"Failed to alloc clipboard memory.", L"Error", MB_OK | MB_ICONERROR);
        CloseClipboard();
        return; // 处理错误
    }
    byte* copyData = reinterpret_cast<byte*>(GlobalLock(copyHandle));
    if (copyData) {
        memcpy(copyData, text.data(), len);
    }
    GlobalUnlock(copyHandle);
    SetClipboardData(CF_UNICODETEXT, copyHandle);
    CloseClipboard();
}

void Util::copyColor(const int& key)
{
    std::wstring tarStr;
    HDC hdcScreen = GetDC(NULL);
    POINT pos;
    GetCursorPos(&pos);
    COLORREF colorNative = GetPixel(hdcScreen, pos.x, pos.y);
    ReleaseDC(NULL, hdcScreen);
    if (key == 0) {
        QColor color(GetRValue(colorNative), GetGValue(colorNative), GetBValue(colorNative));
        tarStr = color.name().toUpper().toStdWString();
    }
    else if (key == 1) {
        tarStr = QString("%1,%2,%3").arg(GetRValue(colorNative)).arg(GetGValue(colorNative)).arg(GetBValue(colorNative)).toStdWString();
    }
    else if (key == 2) {
        QColor cmyk(GetRValue(colorNative), GetGValue(colorNative), GetBValue(colorNative));
        tarStr = QString("%1,%2,%3,%4").arg(cmyk.cyan()).arg(cmyk.magenta()).arg(cmyk.yellow()).arg(cmyk.black()).toStdWString();
    }
    Util::setClipboardText(tarStr);
}

QPoint Util::getQtPoint(int x, int y) {
    POINT point{ .x{x},.y{y} };
    HMONITOR hMonitor = MonitorFromPoint(point, MONITOR_DEFAULTTONEAREST);
    double dpr;
    if (hMonitor) {
        UINT dpiX, dpiY;
        HRESULT hr = GetDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
        if (FAILED(hr)) {
            dpr = 1.0;
        }
        dpr = static_cast<double>(dpiX) / 96.0;
    }
    else {
        dpr = 1.0;
    }
    return QPoint(x / dpr, y / dpr);
}

//QImage Util::printScreen(const int& x, const int& y, const int& w, const int& h)
//{
//    auto screens = QGuiApplication::screens();
//    for (int i = 0; i < screens.count(); i++) {
//        auto screen = screens[i];
//        QRect _screenRect = screen->geometry();
//        if (_screenRect.x() == 0 && _screenRect.y() == 0) {
//            auto dpr = screen->devicePixelRatio();
//            auto img = screen->grabWindow(0,
//                x / dpr,
//                y / dpr,
//                w / dpr,
//                h / dpr).toImage();
//            //img.save("allen123.png");
//            return img;
//        }
//    }
//}
