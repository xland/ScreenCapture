#include "ScreenPrinter.h"
#include <QPainter>
#include <QPixmap>
#include <QFile>
#include <QScreen>
#include <QGuiApplication>
#include <Windows.h>

static ScreenPrinter* instance;

ScreenPrinter::ScreenPrinter(QObject *parent)
    : QObject{parent}
{
}
ScreenPrinter::~ScreenPrinter()
{
}
ScreenPrinter* ScreenPrinter::Get()
{
    return instance;
}
void ScreenPrinter::printScreen()
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
        auto pix = s->grabWindow(0);
        auto g = s->geometry();
        if(g.x() < tempX) tempX = g.x();
        if(g.y() < tempY) tempY = g.y();
        QRect rect(g.x(),g.y(),pix.width(),pix.height());
        rects.append(std::move(rect));
        pixmaps.append(std::move(pix));
    }
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
    scalFactor = (qreal)tempHeight / (qreal)windowHeight;
    scalFactor = round(scalFactor*100)/100;
    //    QFile file("desktopImage.png");
    //    file.open(QIODevice::WriteOnly);
    //    desktopImage.save(&file, "PNG");
}
void ScreenPrinter::enumDesktopWindows()
{
    EnumWindows([](HWND hwnd, LPARAM lparam){
        if(!IsWindowVisible(hwnd)) return TRUE;
        if(IsIconic(hwnd)) return TRUE;
        RECT rect;
        GetWindowRect(hwnd,&rect);
        if(rect.top <= 0 && rect.left <= 0 && rect.bottom <= 0 && rect.right <= 0) return TRUE;
        auto left = qreal(rect.left/instance->scalFactor);
        auto top = qreal(rect.top/instance->scalFactor);
        auto width = qreal((rect.right-rect.left)/instance->scalFactor);
        auto height = qreal((rect.bottom-rect.top)/instance->scalFactor);
        QRect item(left,top,width,height);
        instance->desktopWindowRects.append(std::move(item));
        //todo 枚举每个窗口的子窗口
        return TRUE;
    }, NULL);
}
void ScreenPrinter::Init(QObject *parent)
{
    if(!instance){
        instance = new ScreenPrinter(parent);
        instance->printScreen();
        instance->enumDesktopWindows();
    }

}


