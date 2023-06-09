
#include "ScreenImageProvider.h"

ScreenImageProvider::ScreenImageProvider(QPixmap* desktopImage)
    : QQuickImageProvider(QQuickImageProvider::Pixmap),
    desktopImage{desktopImage}
{
}
ScreenImageProvider::~ScreenImageProvider()
{
}

QPixmap ScreenImageProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    return *desktopImage;
}

