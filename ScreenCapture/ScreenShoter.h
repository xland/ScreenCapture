
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
    QRectF getHighlightWindowRect(const QPointF& point);
    QImage desktopImage;
    QList<QRectF> windowRects;
    QRect screenRect;

private:
    explicit ScreenShoter(QObject* parent = nullptr);
    void shotScreen();
};

#endif // SCREENSHOTER_H
