#include <Windows.h>
#include <format>

#include "Util.h"

//void Util::setCursor(QWidget* widget, Qt::CursorShape cursor)
//{
//    if (widget->cursor().shape() != Qt::CrossCursor) {
//        widget->setCursor(Qt::CrossCursor);
//    }
//}

void Util::CopyText(const std::wstring& text) {
    if (!OpenClipboard(NULL)) {
        MessageBox(NULL, L"Failed to open clipboard.", L"Error", MB_OK | MB_ICONERROR);
        return;
    }
    EmptyClipboard();
    size_t len = (text.size() + 1) * sizeof(wchar_t);
    HANDLE copyHandle = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, len);
    if (copyHandle == NULL)
    {
        MessageBox(NULL, L"Failed to alloc clipboard memory.", L"Error", MB_OK | MB_ICONERROR);
        CloseClipboard();
        return;
    }
    byte* copyData = reinterpret_cast<byte*>(GlobalLock(copyHandle));
    if (copyData) {
        memcpy(copyData, text.data(), len);
    }
    GlobalUnlock(copyHandle);
    HANDLE h = SetClipboardData(CF_UNICODETEXT, copyHandle);
    if (!h) {
        GlobalFree(copyHandle);
    }
    CloseClipboard();
}

std::wstring Util::CreateFileName()
{
    SYSTEMTIME localTime;
    GetLocalTime(&localTime);
    std::wstring name = std::format(L"{}{}{}{}{}{}{}.png", localTime.wYear, localTime.wMonth, localTime.wDay,
        localTime.wHour, localTime.wMinute, localTime.wSecond, localTime.wMilliseconds);
    return name;
}
std::string Util::ToString(std::wstring&& wstr) {
    int count = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstr.length(), NULL, 0, NULL, NULL);
    std::string str(count, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], count, NULL, NULL);
    return str;
}
