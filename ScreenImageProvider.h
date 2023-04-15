
#ifndef SCREENIMAGEPROVIDER_H
#define SCREENIMAGEPROVIDER_H

#include <QQuickImageProvider>


class ScreenImageProvider: public QQuickImageProvider
{
public:
    ScreenImageProvider();
    ~ScreenImageProvider();
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override;
};

#endif // SCREENIMAGEPROVIDER_H
