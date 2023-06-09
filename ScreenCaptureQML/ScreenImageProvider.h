
#ifndef SCREENIMAGEPROVIDER_H
#define SCREENIMAGEPROVIDER_H
#include <QQuickImageProvider>



class ScreenImageProvider:public QQuickImageProvider
{
public:
    ScreenImageProvider(QPixmap* desktopImage);
    ~ScreenImageProvider();
    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize) override;
private:
    QPixmap* desktopImage;
};

#endif // SCREENIMAGEPROVIDER_H
