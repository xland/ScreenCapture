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
    //auto screens = QGuiApplication::screens();
    //for (auto s : screens)
    //{
    //    auto img = s->grabWindow(0).toImage();
    //    desktopImages.append(img);
    //    screenRects.append(std::move(s->geometry()));
    //}


    QRect geometry;
    for (QScreen* const screen : QGuiApplication::screens()) {
        QRect scrRect = screen->geometry();
        scrRect.moveTo(scrRect.x() / screen->devicePixelRatio(),
            scrRect.y() / screen->devicePixelRatio());
        geometry = geometry.united(scrRect);
    }
    QPixmap p(QApplication::primaryScreen()->grabWindow(
        QApplication::desktop()->winId(),
        geometry.x(),
        geometry.y(),
        geometry.width(),
        geometry.height()));
    auto screenNumber = QApplication::desktop()->screenNumber();
    QScreen* screen = QApplication::screens()[screenNumber];
    p.setDevicePixelRatio(screen->devicePixelRatio());
    desktopImage = p.toImage();

    
    //auto screens = QGuiApplication::screens();
    //QList<QRect> tempRects;
    //QList<QPixmap> tempPixmaps;
    //for (auto s : screens) {
    //    tempRects.append(std::move(s->geometry()));
    //    tempPixmaps.append(std::move(s->grabWindow(0)));
    //}
    //QPoint p1(0,0);
    //QPoint p2(0,0);
    //for (size_t i = 0; i < tempRects.count(); i++)
    //{
    //    if (tempRects[i].x() < p1.x()) {
    //        p1.setX(tempRects[i].x());
    //    }
    //    if (tempRects[i].y() < p1.y()) {
    //        p1.setY(tempRects[i].y());
    //    }
    //    if (tempRects[i].bottomRight().x() > p2.x()) {
    //        p2.setX(tempRects[i].bottomRight().x());
    //    }
    //    if (tempRects[i].bottomRight().y() > p2.y()) {
    //        p2.setY(tempRects[i].bottomRight().y());
    //    }
    //}
    //QRect imgRect;
    //imgRect.adjust(p1.x(),p1.y(), p2.x(),p2.y());
    //imgRect.moveTo(0, 0);
    //QTransform transform;
    //transform.translate(std::abs(p1.x()), std::abs(p1.y()));
    //for (size_t i = 0; i < tempRects.count(); i++)
    //{
    //    tempRects[i] = transform.mapRect(tempRects[i]);
    //}
    //desktopImage = QImage(imgRect.size()*1.5, QImage::Format_ARGB32);
    //desktopImage.setDevicePixelRatio(1.5);
    //QPainter painter(&desktopImage);
    //painter.drawPixmap(tempRects[0].topLeft(), tempPixmaps[0]);
    //painter.drawPixmap(tempRects[1].topLeft().x()/1.5+384, tempRects[1].topLeft().y() / 1.5, tempPixmaps[1]);
    //for (size_t i = 0; i < tempRects.count(); i++)
    //{
    //    painter.drawPixmap(tempRects[i].topLeft(), tempPixmaps[i]);
    //}
    //desktopImage.save("desktopImage.png");
    //QTransform transform;
    //transform.translate(p1.x(), p1.y());

    //painter.setTransform(transform)


    



    //painter.setTransform(transform);



    //auto rect = QGuiApplication::primaryScreen()->virtualGeometry(); //-1920,0,4480,1440
    //QList<QRect> rects;
    //QList<QPixmap> pixmaps;
    //int tempX{ 0 }, tempY{ 0 }, tempHeight{ 0 }, tempWidth{ 0 };
    //for (auto s : screens) {
    //    auto pix = s->grabWindow(0);
    //    auto g = s->geometry();
    //    qDebug() << "geometry" << g;
    //    if (g.x() < tempX) tempX = g.x();
    //    if (g.y() < tempY) tempY = g.y();
    //    QRect rect(g.x(), g.y(), pix.width(), pix.height());
    //    rects.append(std::move(rect));
    //    pixmaps.append(std::move(pix));
    //}
    //for (int i = 0; i < rects.count(); ++i) {
    //    rects[i].moveTopLeft({ rects[i].x() - tempX,rects[i].y() - tempY });
    //    if (tempHeight < rects[i].bottom()) tempHeight = rects[i].bottom();
    //    if (tempWidth < rects[i].right()) tempWidth = rects[i].right();
    //}
    //QPixmap pixmap(tempWidth, tempHeight);
    //QPainter painter(&pixmap);
    //for (int i = 0; i < rects.count(); ++i) {
    //    painter.drawPixmap(rects[i], pixmaps[i]);
    //}
    //desktopImage = pixmap.toImage();
    //desktopImage.save("desktopImage.png");

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

