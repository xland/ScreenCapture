
#ifndef SCREENSHOTER_H
#define SCREENSHOTER_H
#include <QPixmap>
#include <QObject>
class ScreenShoter : public QObject
{
    Q_OBJECT
public:
    ~ScreenShoter();
    static void Init(QObject* parent);
    static ScreenShoter* Get();
    static void Dispose();
    int windowX = 0;
    int windowY = 0;
    int windowWidth = 0;
    int windowHeight = 0;
    qreal scalFactor;
    QPixmap* desktopImage;
    QList<QRect> desktopWindowRects;

private:
    explicit ScreenShoter(QObject* parent = nullptr);
    void shotScreen();
    void enumDesktopWindows();
};

#endif // SCREENSHOTER_H