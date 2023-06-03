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
    primaryIndex = QApplication::desktop()->screenNumber();
    QList<QPixmap> pixmaps;
    qreal tempSize = 0;
    int x1 = 999999, y1 = 999999, x2 = -999999, y2 = -999999;
    for (size_t i = 0; i < screens.count(); i++)
    {        
        auto p = screens[i]->grabWindow(0);
        pixmaps.append(std::move(p));
        auto r = screens[i]->devicePixelRatio();
        if (rate < r) {
            rate = r; //找到最大的缩放比例
        }

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
    transform.translate(std::abs(screenRect.x()), std::abs(screenRect.y()));
    QPainter p(&desktopImage);
    for (size_t i = 0; i < pixmaps.count(); i++)
    {
        pixmaps[i].setDevicePixelRatio(rate);
        auto pos = transform.map(rects[i].topLeft());
        p.drawPixmap(pos/rate, pixmaps[i]);
    }
    //desktopImage.save("desktopImage.png");
    //adjustRect(screenRect);
    //screenRect.setSize(desktopImage.size() / rate);
}
void ScreenShoter::adjustRect(QRect& rect)
{
    auto x1 = rect.left();
    auto y1 = rect.top();
    if (isSmallScreenPrimary) { //小屏幕做主屏
        if (rects.count() > 1) {
            if (std::abs(rects[primaryIndex].right() - rects[primaryIndex + 1].left()) < 2) {//640，小屏幕在左边
                auto span = rects[primaryIndex].width() / (rate * 2);
                if (span > 640) span = span / 2;
                rect.moveTo(x1 + span, y1); 
            }
            else if (std::abs(rects[primaryIndex].left() - rects[primaryIndex + 1].right()) < 2) {  //小屏幕在右边
                rect.moveTo(x1, y1);
            }
            else if (std::abs(rects[primaryIndex].bottom() - rects[primaryIndex + 1].top())<2) { //小屏幕在上边
                auto span = rects[primaryIndex].height() / (rate * 2);
                rect.moveTo(x1, y1 + span); 
            }
            else if (std::abs(rects[primaryIndex].top() - rects[primaryIndex + 1].bottom()) < 2) { //小屏幕在下边
                rect.moveTo(x1, y1);
            }
        }
        else
        {
            rect.moveTo(x1 / rate, y1 / rate);
        }
    }
    else {
        rect.moveTo(x1 / rate, y1 / rate);
    }
}
void ScreenShoter::adjustRect2(QRect& rect)
{
    if (rects.count() < 1) return;
    auto x1 = rect.left();
    auto y1 = rect.top();
    if (!isSmallScreenPrimary) { //大屏幕做主屏
        if (std::abs(rects[primaryIndex].left() - rects[primaryIndex + 1].right()) < 2) {//640，小屏幕在左边
            auto span = rects[primaryIndex + 1].width() / (rate * 2);
            if (span > 384) span = span / 2;
            rect.moveTo(x1 - span, y1);
        }
        else if (std::abs(rects[primaryIndex].top() - rects[primaryIndex + 1].bottom()) < 2) { //上
            auto span = rects[primaryIndex + 1].height() / (rate * 2);
            rect.moveTo(x1, y1 - span);
        }
        //else if (std::abs(rects[primaryIndex].right() - rects[primaryIndex + 1].left()) < 2) {  //小屏幕在右边
        //    //rect.moveTo(x1, y1);
        //}
        //else if (std::abs(rects[primaryIndex].bottom() - rects[primaryIndex + 1].top()) < 2) {  //下
        //    //rect.moveTo(x1, y1);
        //}
    }
    else {
        if (std::abs(rects[primaryIndex].right() - rects[primaryIndex + 1].left()) < 2) {//640，小屏幕在左边
            auto span = std::abs(rects[primaryIndex+1].y()) / (rate * 2);
            if (span > 222) span = span / 2;
            rect.moveTo(x1, y1-span);
        }
        else if (std::abs(rects[primaryIndex].bottom() - rects[primaryIndex + 1].top()) < 2) { //小屏幕在上
            auto span = std::abs(rects[primaryIndex + 1].x()) / (rate * 2);
            rect.moveTo(x1 - span, y1);
        }
        else { // 小屏幕在下
            auto spanY = std::abs(rects[primaryIndex + 1].y()) / (rate * 2);
            auto spanX = std::abs(rects[primaryIndex + 1].x()) / (rate * 2);
            if (spanX > 180) spanX = spanX / 2;
            if (spanY > 432) spanY = spanY / 2;
            if (spanY > 216) spanY = spanY / 2;
            rect.moveTo(x1- spanX, y1 - spanY);
        }
    }
}
void ScreenShoter::enumDesktopWindows()
{
    EnumWindows([](HWND hwnd, LPARAM lparam)
        {
            if (!IsWindowVisible(hwnd)) return TRUE;
            if (IsIconic(hwnd)) return TRUE;
            if(GetWindowTextLength(hwnd) < 1) return TRUE;
            RECT rect;
            GetWindowRect(hwnd, &rect);
            QRect item(rect.left, rect.top, rect.right-rect.left,rect.bottom-rect.top);
            if (item.width() <= 6 || item.height() <= 6) {
                return TRUE;
            }
            auto rate = instance->rate;
            QPoint leftTop = instance->transform.map(item.topLeft()/rate);
            item.moveTo(leftTop);
            item.setSize(item.size() / rate);
            instance->adjustRect2(item);
            //std::string title;
            //title.reserve(GetWindowTextLength(hwnd) + 1);
            //GetWindowTextA(hwnd, const_cast<CHAR*>(title.c_str()), title.capacity());            
            //OutputDebugStringA(title.c_str());
            //OutputDebugStringA("\r\n");            
            //QString a = QString::fromLatin1(title.data());
            //if (a.startsWith("asd.txt")) {
            //    QPainter p(&instance->desktopImage);
            //    p.setPen(QPen(QBrush(Qt::red), 3, Qt::SolidLine, Qt::RoundCap));
            //    p.setBrush(Qt::NoBrush);
            //    p.drawRect(item);
            //    //instance->desktopImage.save("desktopImage.png");
            //}
            instance->windowRects.append(std::move(item));
            return TRUE;

         
    }, NULL);
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

