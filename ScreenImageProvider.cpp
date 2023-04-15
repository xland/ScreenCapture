
#include "screenimageprovider.h"
#include "ScreenShoter.h"
#include <QImage>
#include <QCursor>
#include "Cutter.h"

ScreenImageProvider::ScreenImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Image)
{

}
ScreenImageProvider::~ScreenImageProvider()
{
    //会被Qt自动释放
}

QImage ScreenImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    if(id == "background"){
        return ScreenShoter::Get()->desktopImage;
    }else{
        auto shoter = ScreenShoter::Get();
        auto position = QCursor::pos();
        auto mouseX = qRound((qreal)position.x()*shoter->scalFactor);
        auto mouseY = qRound((qreal)position.y()*shoter->scalFactor);
        if(mouseX >= shoter->desktopImage.width()) mouseX = shoter->desktopImage.width()-1;
        if(mouseY >= shoter->desktopImage.height()) mouseY = shoter->desktopImage.height()-1;
        auto color = shoter->desktopImage.pixelColor(mouseX,mouseY);
        Cutter* cutter = Cutter::Get();
        cutter->setMouseX(mouseX);
        cutter->setMouseY(mouseY);
        cutter->setColorHex(color.name(QColor::HexRgb).toUpper());
        cutter->setColorRgb("rgb("+QString::number(color.red())+","+QString::number(color.green())+","+QString::number(color.blue())+")" );
        int x{0},y{0},width{230},height{160};
        x = mouseX - width/16;
        y = mouseY - height/16;
        QRect rect(x,y,width/8,height/8);
        auto result = shoter->desktopImage.copy(rect).scaled(width,height);
        return result;
    }
}

