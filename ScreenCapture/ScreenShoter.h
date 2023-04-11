
#ifndef SCREENSHOTER_H
#define SCREENSHOTER_H
#include <QPixmap>

class ScreenShoter
{
public:
    ~ScreenShoter();
    static void Init();
    static ScreenShoter* Get();
    static void Dispose();
    int absoluteX = 0;
    int absoluteY = 0;
    int totalWidth = 0;
    int totalHeight = 0;
    QPixmap* desktopImage;
    QList<QRect> desktopWindowRects;

private:
    ScreenShoter();
    void shotScreen();
    void enumDesktopWindows();
};

#endif // SCREENSHOTER_H
