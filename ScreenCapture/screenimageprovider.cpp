
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
        auto position = QCursor::pos();
        auto& image = ScreenShoter::Get()->desktopImage;
        Cutter* cutter = Cutter::Get();
        auto color = image.pixelColor(position);
        cutter->setMouseX(position.x());
        cutter->setMouseY(position.y());
        cutter->setColorHex(color.name(QColor::HexRgb).toUpper());
        cutter->setColorRgb("rgb("+QString::number(color.red())+","+QString::number(color.green())+","+QString::number(color.blue())+")" );
        int x{0},y{0},width{230},height{160};
        x = position.x() - width/16;
        y = position.y() - height/16;
        if(x < 0) x = 0;
        if(y < 0) y = 0;
        if(position.x() >= image.width()) x = x - width/16;
        if(position.y() >= image.height()) y = y - height/16;
        QRect rect(x,y,width/8,height/8);
        auto result = image.copy(rect).scaled(width,height);
        return result;
    }

}

