
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

}
ScreenImageProvider::~ScreenImageProvider()
{
    delete desktopImage;
}

QPixmap ScreenImageProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    return *desktopImage;
}

