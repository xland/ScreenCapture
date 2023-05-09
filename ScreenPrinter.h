
#ifndef SCREENPRINTER_H
#define SCREENPRINTER_H


#include <QObject>
#include <QImage>


class ScreenPrinter : public QObject
{
    Q_OBJECT
public:
    ~ScreenPrinter();
    static void Init(QObject *parent);
    static ScreenPrinter* Get();
    int windowX = 0;
    int windowY = 0;
    int windowWidth = 0;
    int windowHeight = 0;
    qreal scalFactor;
    QImage desktopImage;
    QList<QRect> desktopWindowRects;
signals:
private:
    explicit ScreenPrinter(QObject *parent = nullptr);
    void printScreen();
    void enumDesktopWindows();

};

#endif // SCREENPRINTER_H
