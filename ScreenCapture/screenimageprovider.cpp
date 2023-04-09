
#include "screenimageprovider.h"
#include <QScreen>
#include <QPixmap>
#include <QGuiApplication>

ScreenImageProvider::ScreenImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Pixmap)
{
}

QPixmap ScreenImageProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    QScreen* screen = QGuiApplication::primaryScreen();
    QPixmap pixmap = screen->grabWindow();
    return pixmap;
}

