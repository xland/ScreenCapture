
#ifndef CUTTER_H
#define CUTTER_H


#include <QObject>
#include "ScreenShoter.h"

class Cutter : public QObject
{
    Q_OBJECT
    Q_PROPERTY (int absoluteX READ getAbsoluteX CONSTANT)
    Q_PROPERTY (int absoluteY READ getAbsoluteY CONSTANT)
    Q_PROPERTY (int totalWidth READ getTotalWidth CONSTANT)
    Q_PROPERTY (int totalHeight READ getTotalHeight CONSTANT)

    Q_PROPERTY (int cutAreaLeft READ getCutAreaLeft WRITE setCutAreaLeft NOTIFY cutAreaLeftChanged)
    Q_PROPERTY (int cutAreaTop READ getCutAreaTop WRITE setCutAreaTop NOTIFY cutAreaTopChanged)
    Q_PROPERTY (int cutAreaRight READ getCutAreaRight WRITE setCutAreaBottom NOTIFY cutAreaRightChanged)
    Q_PROPERTY (int cutAreaBottom READ getCutAreaBottom WRITE setCutAreaBottom NOTIFY cutAreaBottomChanged)

public:
    explicit Cutter(QObject *parent = nullptr);
    Q_INVOKABLE void getHoveredWindowRect();
signals:
    void cutAreaLeftChanged(int val);
    void cutAreaTopChanged(int val);
    void cutAreaRightChanged(int val);
    void cutAreaBottomChanged(int val);
private:
    int getAbsoluteX() const { return ScreenShoter::Get()->absoluteX; }
    int getAbsoluteY() const { return ScreenShoter::Get()->absoluteY; }
    int getTotalWidth() const { return ScreenShoter::Get()->totalWidth; } //todo
    int getTotalHeight() const { return ScreenShoter::Get()->totalHeight; }//todo

    int getCutAreaLeft(){return cutAreaLeft;}
    int getCutAreaTop(){return cutAreaTop;}
    int getCutAreaRight(){return cutAreaRight;}
    int getCutAreaBottom(){return cutAreaBottom;}

    void setCutAreaLeft(int val){cutAreaLeft = val; emit cutAreaLeftChanged(val);}
    void setCutAreaTop(int val){cutAreaTop = val; emit cutAreaTopChanged(val);}
    void setCutAreaRight(int val){cutAreaRight = val; emit cutAreaRightChanged(val);}
    void setCutAreaBottom(int val){cutAreaBottom = val; emit cutAreaBottomChanged(val);}

    int cutAreaLeft = 0;
    int cutAreaTop = 0;
    int cutAreaRight = 0;
    int cutAreaBottom = 0;

signals:

};

#endif // CUTTER_H
