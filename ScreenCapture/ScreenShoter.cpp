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
    auto screens = QGuiApplication::screens();
    auto primaryIndex = QApplication::desktop()->screenNumber();
    ////x轴排序
    //for (size_t i = 0; i < screens.count() - 1; i++) {
    //    for (size_t j = 0; j < screens.count() - 1 - i; j++) {
    //        if (screens[j]->geometry().x() > screens[j + 1]->geometry().x()) {
    //            auto s = screens[j + 1];
    //            screens[j + 1] = screens[j];
    //            screens[j] = s;
    //        }
    //    }
    //}
    ////y轴排序
    //for (size_t i = 0; i < screens.count() - 1; i++) {
    //    for (size_t j = 0; j < screens.count() - 1 - i; j++) {
    //        if (screens[j]->geometry().x() == screens[j + 1]->geometry().x()) {
    //            if (screens[j]->geometry().y() > screens[j + 1]->geometry().y()) {
    //                auto s = screens[j + 1];
    //                screens[j + 1] = screens[j];
    //                screens[j] = s;
    //            }
    //        }
    //    }
    //}
    QList<QPixmap> pixmaps;
    qreal rate = 1.0;
    qreal physicalDotsPerInch = 0;
    bool isSmallScreenPrimary = false;
    for (size_t i = 0; i < screens.count(); i++)
    {        
        auto p = screens[i]->grabWindow(0);
        pixmaps.append(std::move(p));
        auto r = screens[i]->devicePixelRatio();
        if (physicalDotsPerInch < screens[i]->physicalDotsPerInch()) {
            if (i != primaryIndex) {
                isSmallScreenPrimary = true;
            }
            else {
                isSmallScreenPrimary = false;
            }
            rate = r;
            physicalDotsPerInch = screens[i]->physicalDotsPerInch();
        }
    }
    int x1 = 999999, y1 = 999999, x2 = -999999, y2 = -999999;
    QList<QRect> rects;
    for (size_t i = 0; i < screens.count(); i++)
    {
        auto rect = screens[i]->geometry();
        rect.setSize(rect.size() * screens[i]->devicePixelRatio()); //注意这里，可能需要rate
        if (i != 0) {
            if (rect.x() > rects[i - 1].right()) {
                rect.moveLeft(rects[i - 1].right());
            }
            if (rect.y() > rects[i - 1].bottom()) {
                rect.moveTop(rects[i - 1].bottom());
            }
        }
        if (rect.x() < x1) {
            x1 = rect.x();
        }
        if (rect.y() < y1) {
            y1 = rect.y();
        }
        if (rect.right() > x2) {
            x2 = rect.right();
        }
        if (rect.bottom() > y2) {
            y2 = rect.bottom();
        }
        rects.append(std::move(rect));
    }
    screenRect.adjust(x1,y1,x2,y2);
    desktopImage = QImage(screenRect.size(), QImage::Format_ARGB32);
    desktopImage.setDevicePixelRatio(rate);
    QTransform transform;
    transform.translate(std::abs(screenRect.x()), std::abs(screenRect.y()));
    QPainter p(&desktopImage);
    for (size_t i = 0; i < pixmaps.count(); i++)
    {
        pixmaps[i].setDevicePixelRatio(rate);
        auto pos = transform.map(rects[i].topLeft());
        p.drawPixmap(pos/rate, pixmaps[i]);
    }
    //desktopImage.save("desktopImage.png");
    if (isSmallScreenPrimary) { //小屏幕做主屏
        if (rects.count() > 1) {
            if (rects[primaryIndex].right() == rects[primaryIndex + 1].left()) {
                auto span = rects[primaryIndex].width() / (rate * 2);
                screenRect.moveTo(x1 + span, y1); //640，小屏幕在左边
            }
            else if (rects[1].right() == 0) {  //小屏幕在右边
                screenRect.moveTo(x1, y1);
            }
            else if(rects[primaryIndex].bottom() == rects[primaryIndex + 1].top()) { //上
                auto span = rects[primaryIndex].height() / (rate * 2);
                screenRect.moveTo(x1 , y1 + span); //640，小屏幕在左边
            }
            else if (rects[1].bottom() == 0) {  //小屏幕在右边
                screenRect.moveTo(x1, y1);
            }            
        }
        else
        {
            screenRect.moveTo(x1 / rate, y1 / rate);
        }
    }
    else {
        screenRect.moveTo(x1 / rate, y1 / rate);
    }
    screenRect.setSize(desktopImage.size() / rate);

    
    qDebug() << "ok";
}
void ScreenShoter::enumDesktopWindows()
{
    EnumWindows([](HWND hwnd, LPARAM lparam)
        {
            if (!IsWindowVisible(hwnd)) return TRUE;
            if (IsIconic(hwnd)) return TRUE;            
            RECT rect;
            GetWindowRect(hwnd, &rect);
            if (rect.top <= 0 && rect.left <= 0 && rect.bottom <= 0 && rect.right <= 0) {
                return TRUE;
            }
            QRect item;
            //item.adjust(rect.left/1.5, rect.top/1.5, rect.right/1.5, rect.bottom/1.5);
            item.adjust(rect.left, rect.top, rect.right, rect.bottom);
            if (item.width() <= 2 || item.height() <= 2) {
                return TRUE;
            }

            std::string title;
            title.reserve(GetWindowTextLength(hwnd) + 1);
            GetWindowTextA(hwnd, const_cast<CHAR*>(title.c_str()), title.capacity());
            
            OutputDebugStringA(title.c_str());
            OutputDebugStringA("\r\n");            
//todo 枚举每个窗口的子窗口,其他的append是不是也可以move   

            QString a = QString::fromLatin1(title.data());
            if (a.startsWith("asd.txt")) {
                QPainter p(&instance->desktopImage);
                p.setPen(QPen(QBrush(Qt::red), 3, Qt::SolidLine, Qt::RoundCap));
                p.setBrush(Qt::NoBrush);
                p.drawRect(item);
                //instance->desktopImage.save("desktopImage.png");
            }
            instance->windowRects.append(std::move(item));
            return TRUE;
            //item.setSize({ (int)(item.width() / 1.25),(int)(item.height() / 1.25) });
            //for (QScreen* const screen : QGuiApplication::screens()) {
            //    QRect screenRect = screen->geometry();
            //    qreal rate = screen->devicePixelRatio();
            //    if (screenRect.contains({ rect.left, rect.top })) {
            //        //instance->windowRectRates.append(rate);
            //        item.setSize({ (int)(item.width() / rate),(int)(item.height() / rate) });
            //        break;
            //    }
            //}
         
    }, NULL);
}
QRectF ScreenShoter::getHighlightWindowRect()
{
    for (size_t i = 0; i < windowRects.count(); i++)
    {
        if (windowRects[i].contains(QCursor::pos())) {
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
        //instance->enumDesktopWindows();
    }

}
ScreenShoter* ScreenShoter::Get()
{
    return instance;
}

