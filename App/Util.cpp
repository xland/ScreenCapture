#include <QMessageBox>
#include <QFont>
#include <QFontDatabase>
#include <QFileDialog>
#include <QStandardPaths>
#include <ShellScalingApi.h>

#include "Util.h"
#include "Lang.h"
#include "App/App.h"

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
void Util::imgToClipboard(const QImage& image)
{
    auto [_, compressSize] = App::getCompressVal();
    QImage img;
    if (compressSize != 1.0) {
        img = image.scaled(image.width() * compressSize/100, 
            image.height() * compressSize/100, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }
    else {
        img = image;
    }
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
bool Util::isImagePath(const QString& path)
{
    QFileInfo fileInfo(path);
    QString suffix = fileInfo.suffix().toLower();
    if (fileInfo.isFile() && (suffix == "png" || suffix == "jpg" || suffix == "jpeg" || suffix == "bmp")) {
        return true;
    }
    return false;
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
        tarStr = QString("%1,%2,%3").arg(GetRValue(colorNative)).arg(GetGValue(colorNative)).arg(GetBValue(colorNative)).toStdWString();
    }
    else if (key == 1) {
        QColor color(GetRValue(colorNative), GetGValue(colorNative), GetBValue(colorNative));
        tarStr = color.name().toUpper().toStdWString();
    }
    else if (key == 2) {
        QColor cmyk(GetRValue(colorNative), GetGValue(colorNative), GetBValue(colorNative));
        tarStr = QString("%1,%2,%3,%4").arg(cmyk.cyan()).arg(cmyk.magenta()).arg(cmyk.yellow()).arg(cmyk.black()).toStdWString();
    }
    Util::setClipboardText(tarStr);
}

bool Util::saveToFile(const QImage& img)
{
    auto savePath = App::getSavePath();
	auto [compressQuality, compressSize] = App::getCompressVal();
    if (savePath.toUpper().endsWith(".PNG")) {
		if (compressSize != 100) {
			QImage image = img.scaled(img.width() * compressSize/100, 
                img.height() * compressSize/100,
                Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            image.save(savePath, "PNG", compressQuality);
            return true;
		}
        img.save(savePath, "PNG", compressQuality);
        return true;
    }
    auto fileName = "Img" + QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz") + ".png";    
    QString filePath;
	if (savePath.isEmpty()) {
        QFileDialog dialog(nullptr, Lang::get("saveFile"));
        dialog.setAcceptMode(QFileDialog::AcceptSave);
        dialog.setNameFilter("ScreenCapture (*.png)");
        dialog.setDefaultSuffix("png");
        dialog.setAttribute(Qt::WA_QuitOnClose, false);
        dialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));
        dialog.selectFile(fileName);
        if (dialog.exec() == QDialog::Accepted) {
            filePath = dialog.selectedFiles().first();
        }
        else {
            return false;
        }
    }
    else {
		filePath = QDir::cleanPath(savePath + QDir::separator() + fileName);
    }
    if (compressSize != 1.0) {
        QImage image = img.scaled(img.width() * compressSize/100, 
            img.height() * compressSize/100, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        image.save(filePath, "PNG", compressQuality);
        return true;
    }
    img.save(filePath,"PNG", compressQuality);
    return true;
}
