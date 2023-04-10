#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <Windows.h>
#include "screenimageprovider.h"
#include <QScreen>
#include <QQmlContext>
#include <QPainter>
#include <QPixmap>
#include <QFile>
#include <QWindow>


int absoluteX = 0,absoluteY = 0;

static BOOL CALLBACK enumWindowCallback(HWND hwnd, LPARAM lparam)
{
    if(!IsWindowVisible(hwnd)) return TRUE;
    if(IsIconic(hwnd)) return TRUE;
    RECT rect;
    GetWindowRect(hwnd,&rect);
    if(rect.top <= 0 && rect.left <= 0 && rect.bottom <= 0 && rect.right <= 0) return TRUE;
//    qDebug() << hwnd;
    return TRUE;
}

QPixmap* getDesktopImage()
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
    qreal tempBottom = 0,tempRight = 0;
    for(auto& r:rects){
        r.moveTopLeft({r.x()-absoluteX,r.y()-absoluteY});
        if(tempBottom < r.bottom()) tempBottom = r.bottom();
        if(tempRight < r.right()) tempRight = r.right();
    }
    auto desktopImage = new QPixmap(tempRight,tempBottom);
    QPainter painter(desktopImage);
    for (int i = 0; i < rects.count(); ++i) {
        painter.drawImage(rects.at(i),images.at(i).toImage());
    }
    QFile file("desktopImage.png");
    file.open(QIODevice::WriteOnly);
    desktopImage->save(&file, "PNG");
    return desktopImage;
}

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    EnumWindows(enumWindowCallback, NULL);
    QQmlApplicationEngine engine;
    QPixmap* desktopImage = getDesktopImage();
    engine.addImageProvider(QLatin1String("ScreenImage"), new ScreenImageProvider(desktopImage));
    const QUrl url(u"qrc:/ScreenCapture/Main.qml"_qs);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
        &app, []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.rootContext()->setContextProperty("absoluteX",absoluteX);
    engine.rootContext()->setContextProperty("absoluteY",absoluteY);
    qDebug()<<desktopImage->width();
    qDebug()<<desktopImage->height();
    engine.rootContext()->setContextProperty("totalWidth",desktopImage->width()/1.5); //todo
    engine.rootContext()->setContextProperty("totalHeight",desktopImage->height()/1.5); //todo
    engine.load(url);
    return app.exec();
}
