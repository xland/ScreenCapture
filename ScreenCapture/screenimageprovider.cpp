
#include "screenimageprovider.h"
#include <QScreen>
#include <QPixmap>
#include <QGuiApplication>
#include <QPainter>
#include <QFile>

ScreenImageProvider::ScreenImageProvider(QPixmap* desktopImage)
    : QQuickImageProvider(QQuickImageProvider::Pixmap),
    desktopImage{desktopImage}
{
//    auto screens = QGuiApplication::screens();
//    QList<QRectF> rects;
//    QList<QPixmap> images;
//    qreal tempX = 0,tempY = 0;
//    for(auto s:screens){
//        auto ratio = s->devicePixelRatio();
//        auto g = s->geometry();
//        auto x = g.x();
//        auto y = g.y();
//        auto w = g.width()*ratio;
//        auto h = g.height()*ratio;
//        if(x < tempX) tempX = x;
//        if(y < tempY) tempY = y;
//        QRectF r(x,y,w,h);
//        rects.append(std::move(r));
//        QPixmap pixmap = s->grabWindow();
//        images.append(std::move(pixmap));
//    }
//    qreal tempBottom = 0,tempRight = 0;
//    for(auto& r:rects){
//        r.moveTopLeft({r.x()-tempX,r.y()-tempY});
//        if(tempBottom < r.bottom()) tempBottom = r.bottom();
//        if(tempRight < r.right()) tempRight = r.right();
//    }
//    desktopImage = new QPixmap(tempRight,tempBottom);
//    QPainter painter(desktopImage);
//    for (int i = 0; i < rects.count(); ++i) {
//        painter.drawImage(rects.at(i),images.at(i).toImage());
//    }
//    QFile file("desktopImage.png");
//    file.open(QIODevice::WriteOnly);
//    desktopImage->save(&file, "PNG");
}
ScreenImageProvider::~ScreenImageProvider()
{
    delete desktopImage;
}

QPixmap ScreenImageProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    return *desktopImage;
}

