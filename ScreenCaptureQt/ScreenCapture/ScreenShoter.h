
#ifndef SCREENSHOTER_H
#define SCREENSHOTER_H
#include <QImage>
#include <QObject>
#include <QList>
class ScreenShoter : public QObject
{
    Q_OBJECT
public:
    ~ScreenShoter();
    static void Dispose();
    static void Init(QObject* parent);
    static ScreenShoter* Get();
    void enumDesktopWindows();
    void adjustRect(QRect& rect);
    void adjustRect2(QRect& rect);
    QImage desktopImage;
    QList<QRectF> windowRects;
    QRect screenRect;
    qreal rate = 1.0;
    QTransform transform;

private:
    bool isSmallScreenPrimary = false;
    int primaryIndex = 0;
    QList<QRect> rects;
    explicit ScreenShoter(QObject* parent = nullptr);
    void shotScreen();
};

#endif // SCREENSHOTER_H
