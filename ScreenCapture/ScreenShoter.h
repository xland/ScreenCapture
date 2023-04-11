
#ifndef SCREENSHOTER_H
#define SCREENSHOTER_H
#include <QImage>
#include <QObject>
class ScreenShoter: public QObject
{
    Q_OBJECT
public:
    ~ScreenShoter();
    static void Init(QObject *parent);
    static ScreenShoter* Get();
    static void Dispose();
    int absoluteX = 0;
    int absoluteY = 0;
    int totalWidth = 0;
    int totalHeight = 0;
    QImage desktopImage;
    QList<QRect> desktopWindowRects;

private:
    explicit ScreenShoter(QObject *parent = nullptr);
    void shotScreen();
    void enumDesktopWindows();
};

#endif // SCREENSHOTER_H
