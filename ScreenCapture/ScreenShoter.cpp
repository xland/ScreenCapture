#include "ScreenShoter.h"
#include <QPainter>
#include <QPixmap>
#include <QFile>
#include <QScreen>
#include <QGuiApplication>
#include <Windows.h>

static ScreenShoter* instance;

ScreenShoter::ScreenShoter(QObject *parent)
    : QObject{parent}
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
    QList<QRectF> rects;
    QList<QPixmap> images;
    for(auto s:screens){
        auto ratio = s->devicePixelRatio();
        auto g = s->geometry();
        auto x = g.x();
        auto y = g.y();
        auto w = g.width()*ratio;
        auto h = g.height()*ratio;
        if(x < absoluteX) absoluteX = x;
        if(y < absoluteY) absoluteY = y;
        QRectF r(x,y,w,h);
        rects.append(std::move(r));
        QPixmap pixmap = s->grabWindow();
        images.append(std::move(pixmap));
    }
    for(auto& r:rects){
        r.moveTopLeft({r.x()-absoluteX,r.y()-absoluteY});
        if(totalHeight < r.bottom()) totalHeight = r.bottom();
        if(totalWidth < r.right()) totalWidth = r.right();
    }
    QPixmap pixmap(totalWidth,totalHeight);
    QPainter painter(&pixmap);
    for (int i = 0; i < rects.count(); ++i) {
        painter.drawImage(rects.at(i),images.at(i).toImage());
    }
    desktopImage = pixmap.toImage();
    //    QFile file("desktopImage.png");
    //    file.open(QIODevice::WriteOnly);
    //    desktopImage->save(&file, "PNG");
}
void ScreenShoter::enumDesktopWindows()
{
    EnumWindows([](HWND hwnd, LPARAM lparam){
        if(!IsWindowVisible(hwnd)) return TRUE;
        if(IsIconic(hwnd)) return TRUE;
        RECT rect;
        GetWindowRect(hwnd,&rect);
        if(rect.top <= 0 && rect.left <= 0 && rect.bottom <= 0 && rect.right <= 0) return TRUE;
        QRect item(rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top);
        instance->desktopWindowRects.append(std::move(item));
        //todo 枚举每个窗口的子窗口
        return TRUE;
    }, NULL);
}
void ScreenShoter::Init(QObject *parent)
{
    if(!instance){
        instance = new ScreenShoter(parent);
        instance->enumDesktopWindows();
        instance->shotScreen();
    }

}
ScreenShoter* ScreenShoter::Get()
{
    return instance;
}
