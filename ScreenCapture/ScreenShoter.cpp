#include "ScreenShoter.h"
#include <QPainter>
#include <QPixmap>
#include <QFile>
#include <QScreen>
#include <QGuiApplication>
#include <Windows.h>
#include <qtransform.h>
#include <qapplication.h>
#include <qdesktopwidget.h>
#include <qdebug.h>

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
//#if defined(Q_OS_MACOS)
//    QScreen* currentScreen = QGuiAppCurrentScreen().currentScreen();
//    QPixmap screenPixmap(
//        currentScreen->grabWindow(QApplication::desktop()->winId(),
//            currentScreen->geometry().x(),
//            currentScreen->geometry().y(),
//            currentScreen->geometry().width(),
//            currentScreen->geometry().height()));
//    screenPixmap.setDevicePixelRatio(currentScreen->devicePixelRatio());
//    return screenPixmap;
    int x1=0, y1=0;
    for (QScreen* const screen : QGuiApplication::screens()) {
        auto r = screen->geometry();
        if (r.x() < x1) {
            x1 = r.x();
            if (r.x() + r.width() < 0) {
                x1 -= (r.x() + r.width());
            }
        }
        if (r.y() < y1) {
            y1 = r.y();
            if (r.y() + r.height() < 0) {
                y1 -= (r.y() + r.height());
            }
        }
    }
    QRect rect;
    for (QScreen* const screen : QGuiApplication::screens()) {
        QRect scrRect = screen->geometry();
        scrRect.moveTo(scrRect.x() / screen->devicePixelRatio(),
            scrRect.y() / screen->devicePixelRatio());
        rect = rect.united(scrRect);
    }
    screenRect = screenRect.adjusted(x1,y1,x1+rect.width(), y1+rect.height());
    QPixmap p(QApplication::primaryScreen()->grabWindow(
        QApplication::desktop()->winId(),
        rect.x(),
        rect.y(),
        rect.width(),
        rect.height()));
    auto screenNumber = QApplication::desktop()->screenNumber();
    QScreen* screen = QApplication::screens()[screenNumber];
    p.setDevicePixelRatio(screen->devicePixelRatio());
    desktopImage = p.toImage();

}
void ScreenShoter::enumDesktopWindows()
{
    //EnumWindows([](HWND hwnd, LPARAM lparam)
    //{
    //    if (!IsWindowVisible(hwnd)) return TRUE;
    //    if (IsIconic(hwnd)) return TRUE;
    //    auto scaleFactor = 1.5; //todo每个屏幕不一样
    //    RECT rect;
    //    GetWindowRect(hwnd, &rect);
    //    if (rect.top <= 0 && rect.left <= 0 && rect.bottom <= 0 && rect.right <= 0) return TRUE;
    //    QRect item;
    //    item.adjust(rect.left/scaleFactor, rect.top / scaleFactor, rect.right / scaleFactor, rect.bottom/scaleFactor);
    //    instance->windowRects.append(std::move(item));
    //    //todo 枚举每个窗口的子窗口,其他的append是不是也可以move
    //    return TRUE;
    //}, NULL);
}
QRectF ScreenShoter::getHighlightWindowRect(const QPointF& point)
{
    for (size_t i = 0; i < windowRects.count(); i++)
    {
        if (windowRects[i].contains(point)) {
            return windowRects[i];
        }
    }
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

