
#include "screenimageprovider.h"
#include "ScreenShoter.h"
#include <QPixmap>

ScreenImageProvider::ScreenImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Pixmap)
{

}
ScreenImageProvider::~ScreenImageProvider()
{
}

QPixmap ScreenImageProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    auto image = ScreenShoter::Get()->desktopImage;
    return *image;
}

