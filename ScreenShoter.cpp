#include "ScreenShoter.h"
#include <QPainter>
#include <QPixmap>
#include <QFile>
#include <QScreen>
#include <QGuiApplication>
#include <Windows.h>

static ScreenShoter* instance;

ScreenShoter::ScreenShoter(QObject* parent)
    : QObject{ parent }
{
}
ScreenShoter::~ScreenShoter()
{
}
void ScreenShoter::Dispose()
{
    delete instance;
}

void ScreenShoter::shotScreen()
{
    auto screens = QGuiApplication::screens();
    for (auto s : screens)
    {
        auto pix = s->grabWindow(0);
        desktopImages.append(pix);
        screenRects.append(s->geometry());
    }
}
void ScreenShoter::enumDesktopWindows()
{
//    EnumWindows([](HWND hwnd, LPARAM lparam)
//    {
//        if (!IsWindowVisible(hwnd)) return TRUE;
//        if (IsIconic(hwnd)) return TRUE;
//        RECT rect;
//        GetWindowRect(hwnd, &rect);
//        if (rect.top <= 0 && rect.left <= 0 && rect.bottom <= 0 && rect.right <= 0) return TRUE;
//        auto left = qreal(rect.left / instance->scalFactor);
//        auto top = qreal(rect.top / instance->scalFactor);
//        auto width = qreal((rect.right - rect.left) / instance->scalFactor);
//        auto height = qreal((rect.bottom - rect.top) / instance->scalFactor);
//        QRect item(left, top, width, height);
//        instance->desktopWindowRects.append(std::move(item));
//        //todo 枚举每个窗口的子窗口
//        return TRUE;
//    }, NULL);
}
void ScreenShoter::Init(QObject* parent)
{
    if (!instance)
    {
        instance = new ScreenShoter(parent);
        instance->shotScreen();
        instance->enumDesktopWindows();
    }

}
ScreenShoter* ScreenShoter::Get()
{
    return instance;
}

