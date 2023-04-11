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
    auto rect = QGuiApplication::primaryScreen()->virtualGeometry(); //0,-300,4480,960
    windowX = rect.x();
    windowY = rect.y();
    windowHeight = rect.height();
    windowWidth = rect.width();
    QList<QRect> rects;
    QList<QPixmap> pixmaps;
    int tempX{0},tempY{0},tempHeight{0},tempWidth{0};
    for(auto s:screens){
        auto pix = s->grabWindow();
        auto g = s->geometry();
        if(g.x() < tempX) tempX = g.x();
        if(g.y() < tempY) tempY = g.y();
        QRect rect(g.x(),g.y(),pix.width(),pix.height());
        rects.append(std::move(rect));
        pixmaps.append(std::move(pix));
    }
    windowX += rects[0].width()/2;
    for(int i = 0; i < rects.count(); ++i){
        rects[i].moveTopLeft({rects[i].x()-tempX,rects[i].y()-tempY});
        if(tempHeight < rects[i].bottom()) tempHeight = rects[i].bottom();
        if(tempWidth < rects[i].right()) tempWidth = rects[i].right();
    }
    QPixmap pixmap(tempWidth,tempHeight);
    QPainter painter(&pixmap);
    for(int i = 0; i < rects.count(); ++i){
        painter.drawPixmap(rects[i],pixmaps[i]);
    }
    desktopImage = pixmap.toImage();
//    QFile file("desktopImage.png");
//    file.open(QIODevice::WriteOnly);
//    desktopImage.save(&file, "PNG");
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
